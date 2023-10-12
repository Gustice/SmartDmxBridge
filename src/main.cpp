/**
 * @file main.cpp
 * @author Gustice
 * @brief DMX-Bridge
 * @details Designed to control a DMX-chain in following modes:
 *   - Standalone mode: To control a simple configurable scene with a few slider
 *   - Remote mode (optionally): Can be controlled by a highly computer GUI using ArtNet protocol
 *   - Diagnosis mode (optionally): Can be used to validate DMX-chain bye closing the ring
 * 
 * This device is based on hardware ESP32-POE-ISO 
 *  https://www.olimex.com/Products/IoT/ESP32/ESP32-POE-ISO/open-source-hardware
 * Open Source documentation is on
 *   https://github.com/OLIMEX/ESP32-POE-ISO
 * Olimex also provides Example Projects for this platform
 *   https://github.com/OLIMEX/ESP32-POE/tree/master/SOFTWARE/ESP-IDF
 * 
 * This project is generated with Platform IO, based on Espressif IDF
 * An introduction can be found at
 *   https://docs.platformio.org/en/latest/frameworks/espidf.html
 * ESP-specific configuration can be found at
 *   https://docs.platformio.org/en/latest/platforms/espressif32.html
 * 
 * ESP-IDF was choosen in order to use the device integrated RMII connected LAN8710A-PHY 
 *   as ethernet interface (Arduino Framework relies on SPI connected PHYs and WiFi)
 * 
 * The ArtNet-Interface was tested with [Q Light Controller+](https://www.qlcplus.org/)
 * 
 * Partition table
 * NOTE: The entry prtTbl is not explicitly stated in partition table csv
 * NOTE: There is no particular partition for factory app. only OTA1 and OTA2
 * | Name     |  Type |  SubType |   Offset|      Size|              Description |
 * | -------- | ----- | -------- |  ------ | -------  | ------------------------ |
 * | nvs      |  data |      nvs |         |    0x4000| system / user data       |
 * | otadata  |  data |      ota |         |    0x2000| system                   |
 * | phy_init |  data |      phy |         |    0x1000| system                   |
 * | prtTbl   |   SYS |    TABLE |  0x8000 |    0x1000| partition table          |
 * | config   |  data |   spiffs | 0x10000 |   0x10000| user bin config          |
 * | dataFs   |  data |   spiffs |         |   0xE0000| user file system         |
 * | ota_0    |   app |    ota_0 |      1M |  0x170000| OTA1 partition (factory) |
 * | ota_1    |   app |    ota_1 |         |  0x170000| OTA2 partition           |
 * | coredump |  data | coredump |         |   0x20000| coredump                 |
 * 
 * 
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */

// NOTE: @todo There are stille some beefy ToDos in some underlying components ...

#include "main.hpp"
#include "sdkconfig.h"

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

/****************************************************************************/ /*
 * Configuration of DMX-Bridge
 ******************************************************************************/

/// @brief Station Name for display
const std::string StationName = "DMX-Bridge";
/// @brief Station Version for display
const std::string StationVersion = "V 0.8.0";

constexpr unsigned MinTaskStack = 4096;
constexpr uint16_t SyslogPort = 514;
constexpr uint16_t TelnetPort = 23;
constexpr uint16_t ArtNetPort = arx::artnet::DEFAULT_PORT;

// clang-format off
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
    .intensity{
        .unit = adc_unit_t::ADC_UNIT_2,
        .port = adc_channel_t::ADC_CHANNEL_3 // => GPIO 15
    },
    .ambiente{
        .unit = adc_unit_t::ADC_UNIT_2,
        .port = adc_channel_t::ADC_CHANNEL_6 // => GPIO 14
    }};

EtherPins_t etherPins = {
    .ethPhyAddr = GPIO_NUM_0,
    .ethPhyRst = GPIO_NUM_NC,
    .ethPhyMdc = GPIO_NUM_23,
    .ethPhyMdio = GPIO_NUM_18,
    .ethPhyPower = GPIO_NUM_12,
};
// clang-format on

static const char *TAG = "dmx-bridge";

// forward declarations
static void startDmxMonitor(MonitorType type, std::shared_ptr<TaskControl> token);
static void showSystemHealth();
static void valueRefresherTask(void *);
static void sysLogSocket_task(void *pvParameters);
static void newColorScheme(AmbientColorSet color);
static void dmx_RingMonitor(void *arg);
static void dmx_Monitor(void *arg);

/****************************************************************************/ /*
 * Local instances of Device
 ******************************************************************************/

static DebuggingOptions debugOptions;
static DeviceState deviceState(DeviceState::Mode::StandAlone);
static std::array<uint8_t, 2> relativeIntensity;
static MessageQueue<Display::Message> displayEvents(10);
static MessageQueue<LogMessage> logEvents(32);

static OtaHandler ota("http://192.168.178.10:8070/firmware.bin");
static Dmx512 dmxPort(ioMap.dmx.port, ioMap.dmx.rxPin, ioMap.dmx.txPin);
IpInfo deviceInfo;
static Semaphore ipAddressSem{2, 0}; // Counting semaphore
std::array<uint32_t, 2> dmxUniverses{1, 2};
BinDiff differ(24);
static StageConfig stage = DefaultStageConfig;
AmbientColorSet stageAmbient{stage.colorsPresets[0]};
StageIntensity intensity;

Ui::Config uiConfig{
    .stage = stage,
    .dmx = dmxPort,
    .ota = ota,
    .startMonitor = startDmxMonitor,
    .showHealth = showSystemHealth,
    .debugOptions = debugOptions};

std::shared_ptr<FileAccess> _paramFs;
const std::string configRoot("/spiffs");
const std::string partitionLabel("dataFs");

/****************************************************************************/ /*
 * Callbacks
 ******************************************************************************/

/**
 * @brief Callback for "dump-status" command
 */
static void showSystemHealth() {
    ESP_LOGI(TAG, "System-Stats");
    ESP_LOGI(TAG, "Heap-Info:");
    ESP_LOGI(TAG, " .. free %d / minimum %d", heap_caps_get_free_size(MALLOC_CAP_8BIT),
             heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
}

/**
 * @brief Callback for Got-IP  event
 * 
 * @param info IP and Mask info
 */
static void gotIpCallback(IpInfo info) {
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

/**
 * @brief Callback for ArtNet DMX-socket. Does not send data only updates the image
 * 
 * @param data Pointer to dmx-values
 * @param size length of dmx-values
 */
static void Universe1Callback(const uint8_t *data, const uint16_t size) {
    int len = std::min(static_cast<int>(size), DmxChannelCount);
    dmxPort.set(data, len);
    // NOTE: Do not trigger her dmxPort.send(); Send should be triggered cyclically
}

/**
 * @brief Callback to apply new color scheme
 * 
 * @param color color scheme to be set
 */
static void newColorScheme(AmbientColorSet color) {
    auto fg = color.foregroundColor;
    auto bg = color.backgroundColor;

    stageAmbient.backgroundColor = color.foregroundColor;
    stageAmbient.foregroundColor = color.backgroundColor;

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

/**
 * @brief Callback for "start-monitor" command
 * 
 * @param type 
 * @param token 
 */
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

/**
 * @brief Set ambiente-color callback from web-frontend
 * 
 * @param type foreground/background
 * @param color color
 */
void WebColorCallback(AmbientType type, Color color) {
    if (!deviceState.stateIs(DeviceState::Mode::WebUi)) {
        deviceState.setNewState(DeviceState::Mode::WebUi);
        auto msg = std::make_unique<Display::Message>(Display::Message::Type::UpdateInfo, "in web-mode");
        displayEvents.enqueue(std::move(msg));
    }

    if (type == AmbientType::Foreground) {
        stageAmbient.foregroundColor = color;
    } else {
        stageAmbient.backgroundColor = color;
    }
}

/**
 * @brief Set intensities callback from web-frontend
 * 
 * @param intensities intensities
 */
void WebIntensityCallback(StageIntensity intensities) {
    if (!deviceState.stateIs(DeviceState::Mode::WebUi)) {
        deviceState.setNewState(DeviceState::Mode::WebUi);
        auto msg = std::make_unique<Display::Message>(Display::Message::Type::UpdateInfo, "in web-mode");
        displayEvents.enqueue(std::move(msg));
    }

    intensities = intensities;
}

/****************************************************************************/ /*
 * Sockets
 ******************************************************************************/

/**
 * @brief DMX-Socket for ArtNet TCP/IP
 * 
 * @details Tested with [Q Light Controller+](https://www.qlcplus.org/)
 * @param args ignored task argument
 */
static void dmxSocket_task(void *args) {
    const sockaddr_in dest_addr = createIpV4Config(INADDR_ANY, ArtNetPort);
    while (true) {
        UdpSocket socket(dest_addr, deviceInfo);
        socket.attach();
        ArtnetReceiver artnet(socket);
        artnet.subscribe(dmxUniverses[0], Universe1Callback);

        artnet.parse();
        auto msg = std::make_unique<Display::Message>(Display::Message::Type::UpdateInfo, "DMX-IP-Mode");
        displayEvents.enqueue(std::move(msg));
        deviceState.setNewState(DeviceState::Mode::Remote);
        xTaskCreate(valueRefresherTask, "valueRefresherTask", 4096, nullptr, 5, NULL);

        // parse artnet as long socket is active, then restart socket
        while (socket.isActive()) {
            ESP_LOGD(TAG, "Waiting for data");
            artnet.parse();
        }
        deviceState.fallbackToLast();
    }
    vTaskDelete(NULL);
}

/**
 * @brief Telnet socket via TCP/IP
 * 
 * @details Tested with TeraTerm. This can be conveniently started by VS-Code task
 *   (note to change the device IP in .vscode/settings.json-file). 
 *   The ini-file for TeraTerm only changes CRReceive and CRSend values both to "=CR"
 * @param arg ignored task argument
 */
static void telnetSocket_task(void *arg) {
    TcpSession::Config config{
        .keepAlive = 1,
        .keepIdle = 5,
        .keepInterval = 5,
        .keepCount = 3,
    };
    const sockaddr_in dest_addr = createIpV4Config(INADDR_ANY, TelnetPort);
    while (true) {
        TcpSocket socket(dest_addr, deviceInfo);
        socket.attach();

        // Run telnet-session as long socket is active else restart socket
        while (socket.isActive()) {
            ESP_LOGI(TAG, "Socket waiting for connect");
            TcpSession session(config, socket);

            static LogSession logSession{.latestClient = session.getSource(), .terminate = false};
            xTaskCreate(sysLogSocket_task, "sysLogSocket", 4096, &logSession, 5, NULL);

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

/**
 * @brief Syslog-Task (spawn in on active Telnet session)
 * 
 * @param pvParameters Pointer to LogSession 
 */
static void sysLogSocket_task(void *pvParameters) {
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

/****************************************************************************/ /*
 * Tasks
 ******************************************************************************/

/**
 * @brief Task initializing and refreshing the display
 * 
 * @param arg ignored task argument
 */
static void displayTask(void *arg) {
    ESP_LOGI(TAG, "Starting Display Task");
    static Uart nxtPort(ioMap.display.port, ioMap.display.rxPin, ioMap.display.txPin,
                        Uart::BaudRate::_38400Bd);
    static Display display(nxtPort, StationName, StationVersion, stage.colorsPresets, newColorScheme);

    while (true) {
        display.tick();
        display.processQueue(displayEvents);
        display.setValues(relativeIntensity);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 * @brief Task to run a Ring-Monitor for diagnosis
 * @details Sends currently set values reads them pack and compares with sent values
 *      Sends log if values don't match
 * 
 * @param arg shared-pointer to Task-Control
 */
static void dmx_RingMonitor(void *arg) {
    std::shared_ptr<TaskControl> token(*(std::shared_ptr<TaskControl> *)arg);

    deviceState.setNewState(DeviceState::Mode::TestRing);
    auto msg = std::make_unique<Display::Message>(Display::Message::Type::UpdateInfo, "TEST-MODE");
    displayEvents.enqueue(std::move(msg));

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

/**
 * @brief Task to run a Bus-Monitor for diagnosis
 * @details Reads values from DMX and compares with last read values
 *      Sends log if values don't match
 * 
 * @param arg shared-pointer to Task-Control
 */
static void dmx_Monitor(void *arg) {
    std::shared_ptr<TaskControl> token(*(std::shared_ptr<TaskControl> *)arg);

    deviceState.setNewState(DeviceState::Mode::Sensing);
    auto msg = std::make_unique<Display::Message>(Display::Message::Type::UpdateInfo, "SENS-MODE");
    displayEvents.enqueue(std::move(msg));

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

/**
 * @brief Task for to provide a simple way to cyclically refresh values
 * @note If refresh would be directly driven by the ArtNet-callback events can 
 *   pile up which leads to flashes on the lights
 */
static void valueRefresherTask(void *) {
    while (deviceState.stateIs(DeviceState::Mode::Remote)) {
        // the data might be changed midway, but as the refresh is fast relative to manipulation
        // this won't lead to inconveniences
        dmxPort.send();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    vTaskDelete(NULL);
}

/**
 * @brief Task for standalone mode
 * @details Reads cyclically Potentiometer, sets illumination accordingly 
 *  and propagates values to display
 * 
 * @param arg 
 */
void standAloneTask(void *arg) {
    Adc adcLight{ioMap.intensity.unit, ioMap.intensity.port};
    Adc adcAmbiente{ioMap.ambiente.unit, ioMap.ambiente.port};

    ScaledValue<int> intensityScale{{0, 4095}, {0, 255}};
    ScaledValue<int> displayScale{{0, 255}, {0, 100}};
    RatiometricLightControl lights(stage, stageAmbient);

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

/****************************************************************************/ /*
 * Main
 ******************************************************************************/
/**
 * @brief Entry point for Operating system
 * @details Setup station, spawns tasks and returns
 * 
 */
void app_main(void) {
    ESP_LOGI("INIT", "###################################");
    ESP_LOGI("INIT", "Loading App %s %s", StationName.c_str(), StationVersion.c_str());
    ESP_LOGI("INIT", "###################################");

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
    _paramFs = std::make_shared<FileAccess>(configRoot, partitionLabel);

    try {
        if (_paramFs->checkIfFileExists(DeviceConfigFilename.begin()) != ESP_OK) {
            ESP_LOGW(TAG, "config-file missing");
        } else {
            ESP_LOGI(TAG, "config-file found");

            auto config = _paramFs->readFile(DeviceConfigFilename.begin());
            ESP_LOGI(TAG, "read config: %s", config.c_str());

            if (config.size() != 0) {
                stage = ParamReader::readDeviceConfig(config);
                stageAmbient = stage.colorsPresets[0];
            } else {
                ESP_LOGW(TAG, "Error reading config ...fallback to default values");
            }
        }
    } catch (const std::exception &e) {
        ESP_LOGW(TAG, "%s ...fallback to default values", e.what());
    }

    ota.get_sha256_of_partitions();
    xTaskCreate(displayTask, "displayTask", MinTaskStack * 4, NULL, 10, NULL);
    xTaskCreate(standAloneTask, "standAloneTask", MinTaskStack * 2, nullptr, 5, NULL);

    auto msg =
        std::make_unique<Display::Message>(Display::Message::Type::UpdateInfo, "Standalone-Mode");
    displayEvents.enqueue(std::move(msg));

    ESP_LOGI(TAG, "Setting up Web");
    static WebApi web({*_paramFs, stageAmbient, intensity, WebColorCallback, WebIntensityCallback});
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
    xTaskCreate(telnetSocket_task, "telnetSocket", 4096, nullptr, 5, NULL);
    xTaskCreate(dmxSocket_task, "dmxSocket", 4096, nullptr, 5, NULL);
}
