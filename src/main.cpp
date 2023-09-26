
#include "main.hpp"

#include "etherInit.hpp"
#include "otaUpdate.hpp"

#include "sdkconfig.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "adc.hpp"
#include "cliWrapper.hpp"
#include "dmx_uart.hpp"
#include "tcpSocket.hpp"
#include "udpSocket.hpp"

#include <Artnet.h>
#include <lwip/netdb.h>
#include <sys/param.h>

#include "nvs.h"
#include "nvs_flash.h"
#include <sys/socket.h>

#include "BinDiff.hpp"
#include "ScaledValue.hpp"
#include "displayWrapper.hpp"
#include "ratioControl.hpp"
#include "semaphore.hpp"
#include <sys/time.h>

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

const std::string StationName = "DMX-Bridge";
const std::string StationVersion = "T 0.5.0";
constexpr unsigned MinTaskStack = 4096;
constexpr uint16_t SyslogPort = 514;

constexpr DeviceIoMap ioMap{.dmx{
                                .port = 2,
                                .rxPin = GPIO_NUM_32,
                                .txPin = GPIO_NUM_33,
                            },
                            .display{
                                .port = 1,
                                .rxPin = GPIO_NUM_36,
                                .txPin = GPIO_NUM_4,
                            },
                            .intensity{
                                .unit = adc_unit_t::ADC_UNIT_2,
                                .port = adc_channel_t::ADC_CHANNEL_3 // => GPIO 15
                            },
                            .ambiente{
                                .unit = adc_unit_t::ADC_UNIT_2,
                                .port = adc_channel_t::ADC_CHANNEL_6 // => GPIO 14
                            }};

static StageConfig stage{.weightsLights{
                             /*AmbienteGrp 1*/ 0,  0,   0,
                             /*Empty*/ 0,          0,
                             /*AmbienteGrp 2*/ 0,  0,   0,
                             /*Front-Colored */ 0, 0,   0,   255, 125, 0,
                             /*Empty*/ 0,          0,
                             /*Lights*/ 255,       0,   255, 0,
                             /*Halogen*/ 255,      255, 255, 255,
                         },
                         .channelsForeground{1, 2, 3},
                         .channelsBackground{6, 7, 8},
                         .colors{.foregroundColor{255, 64, 0}, .backgroundColor{0, 64, 255}}};

ColorPresets stagePresets{
    .preset1{
        .foregroundColor{255, 0, 0},
        .backgroundColor{0, 127, 127},
    },
    .preset2{
        .foregroundColor{0, 255, 0},
        .backgroundColor{127, 0, 127},
    },
    .preset3{
        .foregroundColor{0, 0, 255},
        .backgroundColor{127, 127, 0},
    },
};

EtherPins_t etherPins = {
    .ethPhyAddr = GPIO_NUM_0,
    .ethPhyRst = GPIO_NUM_NC,
    .ethPhyMdc = GPIO_NUM_23,
    .ethPhyMdio = GPIO_NUM_18,
    .ethPhyPower = GPIO_NUM_12,
};

DebuggingOptions debugOptions;

DeviceState deviceState(DeviceState::Mode::StandAlone);
std::array<uint8_t, 2> relativeIntensity;
MessageQueue<Display::Message> displayEvents(10);
MessageQueue<LogMessage> logEvents(32);

OtaHandler ota("http://192.168.178.10:8070/dmx_bridge.bin");
static const char *TAG = "dmx-bridge";
static Dmx512 dmxPort(ioMap.dmx.port, ioMap.dmx.rxPin, ioMap.dmx.txPin);

void startDmxMonitor(MonitorType type, std::shared_ptr<TaskControl> token);

void showSystemHealth() {
    ESP_LOGI(TAG, "System-Stats");
    ESP_LOGI(TAG, "Heap-Info:");
    ESP_LOGI(TAG, " .. free %d / minimum %d", heap_caps_get_free_size(MALLOC_CAP_8BIT),
             heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
}

Ui::Config uiConfig{
    .stage = stage,
    .dmx = dmxPort,
    .ota = ota,
    .startMonitor = startDmxMonitor,
    .showHealth = showSystemHealth,
    .debugOptions = debugOptions
};

IpInfo deviceInfo;
static Semaphore ipAddressSem{2, 0}; // Counting semaphore

void gotIpCallback(IpInfo info) {
    ipAddressSem.give();

    auto &ip = info.address;
    std::string ipStr = std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." +
                        std::to_string(ip[2]) + "." + std::to_string(ip[3]);

    auto msg = std::make_unique<Display::Message>(Display::Message::Type::UpdatedIp, ipStr);
    displayEvents.enqueue(std::move(msg));
    auto log = std::make_unique<LogMessage>(LogMessage::Type::Event, "Got new Ip" + ipStr);
    logEvents.enqueue(std::move(log));
    deviceInfo = info;
}

std::array<uint32_t, 2> dmxUniverses{1, 2};
BinDiff differ(24);

static void valueRefresherTask(void *) {
    while (deviceState.stateIs(DeviceState::Mode::Remote)) {
        // the data might be changed midway, but as the refresh is fast relative to manipulation
        // this won't lead to inconveniences
        dmxPort.send();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    vTaskDelete(NULL);
}

void Universe1Callback(const uint8_t *data, const uint16_t size) {
    int len = std::min(static_cast<int>(size), DmxChannelCount);
    dmxPort.set(data, len);
    // dmxPort.send(); Send should be triggered cyclically
}

static void dmxSocket(void *args) {
    const sockaddr_in dest_addr = createIpV4Config(INADDR_ANY, arx::artnet::DEFAULT_PORT);
    while (true) {
        UdpSocket socket(dest_addr, deviceInfo);
        socket.attach();
        ArtnetReceiver artnet(socket);
        artnet.subscribe(dmxUniverses[0], Universe1Callback);

        artnet.parse();
        auto msg =
            std::make_unique<Display::Message>(Display::Message::Type::UpdateInfo, "DMX-IP-Mode");
        displayEvents.enqueue(std::move(msg));
        deviceState.setNewState(DeviceState::Mode::Remote);
        xTaskCreate(valueRefresherTask, "valueRefresherTask", 4096, nullptr, 5, NULL);

        while (socket.isActive()) {
            ESP_LOGD(TAG, "Waiting for data");
            artnet.parse();
        }
        deviceState.fallbackToLast();
    }
    vTaskDelete(NULL);
}

static void sysLogTask(void *pvParameters) {
    auto param = static_cast<LogSession *>(pvParameters);
    ESP_LOGI(TAG, "Syslog task started ... client");

    char _addr_str[24];
    inet_ntoa_r((&param->latestClient)->sin_addr, _addr_str, sizeof(_addr_str) - 1);
    ESP_LOGI(TAG, "Telnet activity registered ... starting Syslog to %s", _addr_str);
    sockaddr_in dest_addr = createIpV4Config(0, SyslogPort);
    dest_addr.sin_addr.s_addr = param->latestClient.sin_addr.s_addr;
    while (!param->terminate) {
        UdpSocket socket(dest_addr, deviceInfo);
        while (!param->terminate && socket.isActive()) {
            auto log = logEvents.dequeue();
            socket.write(log.get()->message, dest_addr);
        }
    }
    vTaskDelete(NULL);
}

static void tcp_server_task(void *arg) {
    constexpr uint16_t TelnetPort = 23;
    TcpSession::Config config{
        .keepAlive = 1,
        .keepIdle = 5,
        .keepInterval = 5,
        .keepCount = 3,
    };
    const sockaddr_in dest_addr = createIpV4Config(INADDR_ANY, TelnetPort);
    while (true) {
        TcpSocket socket(dest_addr, deviceInfo);
        while (socket.isActive()) {
            ESP_LOGI(TAG, "Socket waiting for connect");
            TcpSession session(config, socket);

            static LogSession logSession{.latestClient = session.getSource(), .terminate = false};
            xTaskCreate(sysLogTask, "sysLogTask", 4096, &logSession, 5, NULL);

            Ui ui(session, uiConfig);
            if (!session.isActive())
                break;

            while (session.isActive()) {
                ui.process();
            }
            logSession.terminate = true;
        }
    }
    vTaskDelete(NULL);
}

void newColorScheme(AmbientColorSet color) {
    auto fg = color.foregroundColor;
    auto bg = color.backgroundColor;

    stage.colors.backgroundColor = color.foregroundColor;
    stage.colors.foregroundColor = color.backgroundColor;

    if (debugOptions.infos) {
        std::stringstream msg;
        msg << "Setting color to fg: " << fg.red << " " << fg.green << " " << fg.blue;
        msg << "   bg:" << bg.red << " " << bg.green << " " << bg.blue;
        auto log = std::make_unique<LogMessage>(LogMessage::Type::Meas, std::move(msg.str()));
        logEvents.enqueue(std::move(log));
    }
    ESP_LOGI(TAG, "Setting color to fg r=%d g=%d b=%d   bg r=%d g=%d b=%d ", fg.red, fg.green,
             fg.blue, bg.red, bg.green, bg.blue);
}

static void displayTask(void *arg) {
    ESP_LOGI(TAG, "Starting Display Task");
    static Uart nxtPort(ioMap.display.port, ioMap.display.rxPin, ioMap.display.txPin,
                        Uart::BaudRate::_38400Bd);
    static Display display(nxtPort, StationName, StationVersion, stagePresets, newColorScheme);

    while (true) {
        display.tick();
        display.processQueue(displayEvents);
        display.setValues(relativeIntensity);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

struct MonitorConfig {
    Dmx512 &dmx;
    std::function<void(std::string &)> onWrite;
};

static void dmx_RingMonitor(void *arg) {
    std::shared_ptr<TaskControl> token(*(std::shared_ptr<TaskControl> *)arg);

    deviceState.setNewState(DeviceState::Mode::TestRing);
    while (!token->isCanceled()) {
        auto sent = dmxPort.getValues();
        dmxPort.send();
        auto received = dmxPort.receive();

        if (received.size() < sent.values.size()) {
            ESP_LOGW(TAG, "DMX-Monitor, incomplete data received (%d received):", received.size());
        }
        auto res = differ.compareBytes(sent.values, received, {"sent, received"});
        if (!res.areSame) {
            auto logD = std::make_unique<LogMessage>(
                LogMessage::Type::Meas, "DMX-Monitor, error in transmit :\n " + res.diff);
            logEvents.enqueue(std::move(logD));
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    deviceState.fallbackToLast();
    vTaskDelete(nullptr);
}

static void dmx_Monitor(void *arg) {
    std::shared_ptr<TaskControl> token(*(std::shared_ptr<TaskControl> *)arg);

    deviceState.setNewState(DeviceState::Mode::Sensing);
    auto lastReceived = dmxPort.receive();
    while (!token->isCanceled()) {

        auto received = dmxPort.receive();
        auto res = differ.compareBytes(lastReceived, received, {"last, new"});
        if (!res.areSame) {
            auto logD = std::make_unique<LogMessage>(
                LogMessage::Type::Meas, "DMX-Monitor, error in transmit :\n " + res.diff);
            logEvents.enqueue(std::move(logD));
            lastReceived = received;
        }
    }
    deviceState.fallbackToLast();
    vTaskDelete(nullptr);
}

void startDmxMonitor(MonitorType type, std::shared_ptr<TaskControl> token) {
    switch (type) {
    case MonitorType::RingCheck:
        ESP_LOGI(TAG, "Ring-Monitor task");
        xTaskCreate(dmx_RingMonitor, "dmx_RingMonitor", 4096, &token, 5, NULL);
        break;

    case MonitorType::Sensing:
        ESP_LOGI(TAG, "Sens-Monitor task");
        xTaskCreate(dmx_Monitor, "dmx_Monitor", 4096, &token, 5, NULL);
        break;
    }
}

void standAloneTask(void *arg) {
    Adc adcLight{ioMap.intensity.unit, ioMap.intensity.port};
    Adc adcAmbiente{ioMap.ambiente.unit, ioMap.ambiente.port};

    ScaledValue<int> intensityScale{{0, 4095}, {0, 255}};
    ScaledValue<int> displayScale{{0, 255}, {0, 100}};
    RatiometricLightControl lights(stage);
    StageIntensity intensity;

    while (true) {
        intensity.illumination = intensityScale.scale(adcLight.readValue());
        intensity.ambiente = intensityScale.scale(adcAmbiente.readValue());

        relativeIntensity[0] = displayScale.scale(intensity.illumination);
        relativeIntensity[1] = displayScale.scale(intensity.ambiente);

        if (deviceState.stateIs(DeviceState::Mode::StandAlone)) {
            auto values = lights.update(intensity);
            dmxPort.set(values.data(), StageChannelsCount);
            dmxPort.send();

            if (debugOptions.standAlone) {
                std::stringstream msg;
                msg << "Dmx Data (ch 1 .. 24): " << std::hex;
                for (auto &&i : values) {
                    msg << "0x" << i << " ";
                }
                auto log =
                    std::make_unique<LogMessage>(LogMessage::Type::Meas, std::move(msg.str()));
                logEvents.enqueue(std::move(log));
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initEthernetHardware(etherPins, gotIpCallback);

    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 1.OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // 2.NVS partition contains data in new format and cannot be recognized by this version of
        // code. If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ota.get_sha256_of_partitions();
    xTaskCreate(displayTask, "displayTask", MinTaskStack * 4, NULL, 10, NULL);
    xTaskCreate(standAloneTask, "standAloneTask", MinTaskStack * 2, nullptr, 5, NULL);

    auto msg =
        std::make_unique<Display::Message>(Display::Message::Type::UpdateInfo, "Standalone-Mode");
    displayEvents.enqueue(std::move(msg));

    ESP_LOGI(TAG, "Waiting for IP(s)");
    ipAddressSem.take(portMAX_DELAY);

    // iterate over active interfaces, and print out IPs of "our" netifs
    esp_netif_t *netif = NULL;
    esp_netif_ip_info_t ip;
    for (int i = 0; i < esp_netif_get_nr_of_ifs(); ++i) {
        netif = esp_netif_next(netif);
        if (is_our_netif(TAG, netif)) {
            ESP_LOGI(TAG, "Connected to %s", esp_netif_get_desc(netif));
            ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip));
            ESP_LOGI(TAG, "- IPv4 address: " IPSTR, IP2STR(&ip.ip));
        }
    }
    xTaskCreate(tcp_server_task, "tcp_server", 4096, nullptr, 5, NULL);
    xTaskCreate(dmxSocket, "dmxSocket", 4096, nullptr, 5, NULL);
}
