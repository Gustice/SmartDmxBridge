
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

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

const std::string StationName = "DMX-Bridge";
const std::string StationVersion = "T 0.5.0";

constexpr DeviceIoMap ioMap{
    .dmx{
        .port = 2,
        .rxPin = GPIO_NUM_32,
        .txPin = GPIO_NUM_33,
    },
    .display{
        .port = 1,
        .rxPin = GPIO_NUM_36,
        .txPin = GPIO_NUM_4,
    },
    .intensity {
        .unit = adc_unit_t::ADC_UNIT_2,
        .port = adc_channel_t::ADC_CHANNEL_3 // => GPIO 15
    },
    .ambiente {
        .unit = adc_unit_t::ADC_UNIT_2,
        .port = adc_channel_t::ADC_CHANNEL_6 // => GPIO 14
    }
};


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

DeviceState deviceState(DeviceState::Mode::StandAlone);
std::array<uint8_t, 2> intensities;

OtaHandler ota("http://192.168.178.10:8070/dmx_bridge.bin");
static const char *TAG = "dmx-bridge";
DmxChannels channels;

static Dmx512 dmxPort(ioMap.dmx.port, ioMap.dmx.rxPin, ioMap.dmx.txPin);

void startDmxMonitor(std::function<void(std::string &)>);
void stopDmxMonitor();

void showSystemHealth() {
    ESP_LOGI(TAG, "System-Stats");
    ESP_LOGI(TAG, "Heap-Info:");
    ESP_LOGI(TAG, " .. free %d / minimum %d", heap_caps_get_free_size(MALLOC_CAP_8BIT),
             heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
}

Ui::Config uiConfig{
    .stage = stage,
    .channels = channels,
    .ota = ota,
    .startMonitor = startDmxMonitor,
    .stopMonitor = stopDmxMonitor,
    .showHealth = showSystemHealth,
};

#define NR_OF_IP_ADDRESSES_TO_WAIT_FOR (2)

IpInfo deviceInfo;

static xSemaphoreHandle s_semph_get_ip_addrs =
    xSemaphoreCreateCounting(NR_OF_IP_ADDRESSES_TO_WAIT_FOR, 0);
bool notifyDisplay = false;
void gotIpCallback(IpInfo defInfo) {
    xSemaphoreGive(s_semph_get_ip_addrs);
    notifyDisplay = true;
    deviceInfo = defInfo;
}

std::array<uint32_t, 2> dmxUniverses{1, 2};

void Universe1Callback(const uint8_t *data, const uint16_t size) {
    int len = std::min(static_cast<int>(size), DmxChannelCount);
    dmxPort.set(data, len);
    dmxPort.send();
}

static void dmxSocket(void *args) {
    while (true) {
        struct sockaddr_in dest_addr_ip4 {
            .sin_len = sizeof(sockaddr_in), .sin_family = AF_INET,
            .sin_port = htons(arx::artnet::DEFAULT_PORT),
            .sin_addr{
                .s_addr = htonl(INADDR_ANY),
            },
            .sin_zero {
                0
            }
        };

        UdpSocket socket(dest_addr_ip4, deviceInfo);
        // Note: The constructor actually blocks until the socket is established
        // Todo: Change constructor to non blocking and blocking part
        deviceState.setNewState(DeviceState::Mode::Remote);
        ArtnetReceiver artnet(socket);
        artnet.subscribe(dmxUniverses[0], Universe1Callback);

        while (socket.isActive()) {
            ESP_LOGD(TAG, "Waiting for data");
            artnet.parse();
        }
        deviceState.fallbackToLast();
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
    struct sockaddr_in dest_addr_ip4 {
        .sin_len = sizeof(sockaddr_in), .sin_family = AF_INET, .sin_port = htons(TelnetPort),
        .sin_addr{
            .s_addr = htonl(INADDR_ANY),
        },
        .sin_zero {
            0
        }
    };

    while (true) {
        TcpSocket socket(dest_addr_ip4, deviceInfo);
        while (socket.isActive()) {
            ESP_LOGI(TAG, "Socket waiting for connect");
            TcpSession session(config, socket);
            Ui ui(session, uiConfig);
            if (!session.isActive())
                break;

            while (session.isActive()) {
                ui.process();
            }
        }
    }
    vTaskDelete(NULL);
}

void newColorScheme(AmbientColorSet color) {
    auto fg = color.foregroundColor;
    auto bg = color.backgroundColor;

    stage.colors.backgroundColor = color.foregroundColor;
    stage.colors.foregroundColor = color.backgroundColor;

    ESP_LOGI(TAG, "Setting color to fg r=%d g=%d b=%d   bg r=%d g=%d b=%d ", fg.red, fg.green,
             fg.blue, bg.red, bg.green, bg.blue);
}

static void displayTask(void *arg) {
    ESP_LOGI(TAG, "Starting Display Task");
    static Uart nxtPort(ioMap.display.port, ioMap.display.rxPin, ioMap.display.txPin,
                        Uart::BaudRate::_38400Bd);
    static Display display(nxtPort, StationName, StationVersion, stagePresets, newColorScheme);

    while (!notifyDisplay) {
        display.tick();
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    {
        auto &ip = deviceInfo.address;
        display.setIp(std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." +
                      std::to_string(ip[2]) + "." + std::to_string(ip[3]));
    }

    while (true) {
        display.tick();
        vTaskDelay(pdMS_TO_TICKS(50));
        display.tick();
        display.setValues(intensities);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

struct MonitorConfig {
    Dmx512 &dmx;
    std::function<void(std::string &)> onWrite;
};

bool stopRequested = false;
static void dmx_Monitor(void *arg) {
    MonitorConfig *config = (MonitorConfig *)arg;
    Dmx512 &dmx = config->dmx;

    BinDiff differ(24);
    std::vector<uint8_t> sent(49, 0);
    while (true) {
        dmx.set(channels.values.data(), channels.values.size());
        dmx.send();
        auto received = dmx.get();

        std::copy(channels.values.cbegin(), channels.values.cend(), sent.begin() + 1);

        if (received.size() < channels.values.size()) {
            ESP_LOGW(TAG, "DMX-Monitor, incomplete data received (%d received):", received.size());
        }
        auto res = differ.compare(sent, received);
        if (!res.areSame) {
            ESP_LOGE(TAG, "DMX-Monitor, error in transmit :\n %s", res.diff.c_str());
            ESP_LOGI(TAG, "DMX-Monitor, sent -> %d e: %s ..", sent.size(),
                     differ.stringify_list(sent.cbegin(), sent.cbegin() + 13).c_str());
            ESP_LOGI(TAG, "DMX-Monitor, got  <- %d e: %s ..", received.size(),
                     differ
                         .stringify_list(received.cbegin(),
                                         received.cbegin() + std::min(13u, received.size()))
                         .c_str());
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (stopRequested) {
            break;
        }
    }
    stopRequested = false;
}

void startDmxMonitor(std::function<void(std::string &)> onWrite) {
    ESP_LOGI(TAG, "Starting Monitor task");
    MonitorConfig config{
        .dmx = dmxPort,
        .onWrite = onWrite,
    };

    xTaskCreate(dmx_Monitor, "dmx_Monitor", 4096, &config, 5, NULL);
}
void stopDmxMonitor() {
    ESP_LOGI(TAG, "Stopping Monitor task");
    stopRequested = true;
}

void standAloneTask(void *arg) {

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
    xTaskCreate(displayTask, "displayTask", 8192, NULL, 10, NULL);

    ESP_LOGI(TAG, "Waiting for IP(s)");
    xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY);

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
ESP_LOGW(TAG, "HERE1");
    xTaskCreate(tcp_server_task, "tcp_server", 4096, nullptr, 5, NULL);
    ESP_LOGW(TAG, "HERE2");
    xTaskCreate(dmxSocket, "dmxSocket", 4096, nullptr, 5, NULL);

    ESP_LOGW(TAG, "HERE3"); 

    Adc adcLight{ioMap.intensity.unit, ioMap.intensity.port};
    Adc adcAmbiente{ioMap.ambiente.unit, ioMap.ambiente.port};

ESP_LOGW(TAG, "HERE4");
    ScaledValue<int> intensityScale{{0, 4095}, {0, 255}};
    RatiometricLightControl lights(stage);
    StageIntensity intensity;

    while (true) {
        intensity.illumination = intensityScale.scale(adcLight.readValue());
        intensity.ambiente = intensityScale.scale(adcAmbiente.readValue());

        intensities[0] = intensity.illumination;
        intensities[1] = intensity.ambiente;

        auto values = lights.update(intensity);

        // if (deviceState.stateIs(DeviceState::Mode::StandAlone)) {
            dmxPort.set(values.data(), StageChannelsCount);
            dmxPort.send();

            ESP_LOGI(TAG, "Dmx Data: %02x%02x%02x %02x%02x %02x%02x%02x %02x%02x%02x%02x%02x%02x %02x%02x %02x%02x%02x%02x %02x%02x%02x%02x", 
            values[0],values[1],values[2],values[3],values[4],values[5],values[6],values[7],values[8],values[9],values[10],
            values[11],values[12],values[13],values[14],values[15],values[16],values[17],values[18],values[19],values[20],
            values[21],values[22],values[23]);
        // }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
