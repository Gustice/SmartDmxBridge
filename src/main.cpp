#include "etherInit.hpp"
#include "otaUpdate.hpp"

#include "sdkconfig.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "cliWrapper.hpp"
#include "dmxInterface.hpp"
#include "tcpSocket.hpp"
#include "udpSocket.hpp"
#include "uart.hpp"
#include "adc.hpp"

#include <Artnet.h>
#include <lwip/netdb.h>
#include <sys/param.h>

#include "nvs.h"
#include "nvs_flash.h"
#include <sys/socket.h>

#include "displayWrapper.hpp"
#include "ScaledValue.hpp"

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

const std::string StationName = "DMX-Bridge";
const std::string StationVersion = "T 0.5.0";

std::array<uint8_t,2> intensities;


static StageConfig stage {
    .weightsLights {
        /*AmbienteGrp 1*/ 0,0,0, 
        /*Empty*/ 0,0, 
        /*AmbienteGrp 2*/ 0,0,0, 
        /*Front-Colored */ 0,0,0,255,125,0,
        /*Empty*/ 0,0,
        /*Lights*/ 255,0, 255,0,
        /*Halogen*/ 255,255, 255,255,
        },
    .channelsForeground {1,2,3},
    .channelsBackground {6,7,8},
    .colors{
        .foregroundColor{255,64,0},
        .backgroundColor{0,64,255}
    }
};

ColorPresets stagePresets {
    .preset1 {
        .foregroundColor{255,0,0},
        .backgroundColor{0,127,127},
    },
    .preset2 {
        .foregroundColor{0,255,0},
        .backgroundColor{127,0,127},
    },
    .preset3 {
        .foregroundColor{0,0,255},
        .backgroundColor{127,127,0},
    },
};

EtherPins_t etherPins = {
    .ethPhyAddr = GPIO_NUM_0,
    .ethPhyRst = GPIO_NUM_NC,
    .ethPhyMdc = GPIO_NUM_23,
    .ethPhyMdio = GPIO_NUM_18,
    .ethPhyPower = GPIO_NUM_12,
};


OtaHandler ota("http://192.168.178.10:8070/dmx_bridge.bin");
static const char *TAG = "dmx-bridge";
DmxChannels channels;

static void interfaceTask(void *arg) {
    static Uart mntPort(1, GPIO_NUM_36, GPIO_NUM_4, Uart::BaudRate::_115200Bd);
    Cli shell(mntPort, Ui::onCommand);
    Ui ui(shell, stage, channels, ota);
    while (1) {
        shell.process();
    }
}


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

uint32_t universe1 = 1; // 0 - 15
uint32_t universe2 = 2; // 0 - 15

DmxInterface * dmxInterface;
void Universe1Callback(const uint8_t *data, const uint16_t size) {
    int len = size;
    if (len > DmxChannelCount)
        len = DmxChannelCount;
    
    dmxInterface->set(data, size);
    dmxInterface->send();
}

static void dmxSocket(void *args) {
    dmxInterface = (DmxInterface *)args;
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
        ArtnetReceiver artnet(socket);
        artnet.subscribe(universe1, Universe1Callback);

        while (socket.isActive()) {
            ESP_LOGD(TAG, "Waiting for data");
            artnet.parse();
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
            Cli shell(session, Ui::onCommand);
            Ui ui(shell, stage, channels, ota);
            if (!session.isActive())
                break;

            while (session.isActive()) {
                shell.process();
            }
        }
    }
    vTaskDelete(NULL);
}

static void dmx_Listener(void *arg) {
    DmxInterface *dmx = (DmxInterface *)arg;
    while (1) {
        auto bytes = dmx->receive();

        if (bytes.size() > 0) {
        ESP_LOGI(TAG, "DMX-Data received (%d bytes): %d %d %d %d .. %d", bytes.size(), bytes[0],
                 bytes[1], bytes[2], bytes[3], bytes.back());
        }
       
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void newColorScheme(AmbientColorSet color) {
    auto fg = color.foregroundColor;
    auto bg = color.backgroundColor;

    ESP_LOGI(TAG, "Setting color to fg r=%d g=%d b=%d   bg r=%d g=%d b=%d ", fg.red, fg.green, fg.blue, bg.red, bg.green, bg.blue);
}


static void displayTask(void *arg) {
    ESP_LOGI(TAG, "Starting Display Task");
    static Uart nxtPort(1, GPIO_NUM_36, GPIO_NUM_4, Uart::BaudRate::_38400Bd);
    static Display display(nxtPort, StationName, StationVersion, stagePresets, newColorScheme);

    while (!notifyDisplay) {
        display.tick();
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    {
        auto & ip = deviceInfo.address;
        display.setIp(std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." + std::to_string(ip[2]) + "." + std::to_string(ip[3]) );
    }

    while (true) {
        display.tick();
        vTaskDelay(pdMS_TO_TICKS(50));
        display.tick();
        display.setValues(intensities);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void) {
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initEthernetHardware(etherPins, gotIpCallback);
    // xTaskCreate(interfaceTask, "uart_interfaceTask", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
    xTaskCreate(displayTask, "displayTask", 8192, NULL, 10, NULL);

    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 1.OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // 2.NVS partition contains data in new format and cannot be recognized by this version of code.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ota.get_sha256_of_partitions();

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

    xTaskCreate(tcp_server_task, "tcp_server", 4096, nullptr, 5, NULL);
    
    static Uart dmxPort(2, GPIO_NUM_32, GPIO_NUM_33, Uart::BaudRate::_250000Bd, 128, Uart::StopBits::_2sb);
    static DmxInterface dmxInterface(dmxPort);
    xTaskCreate(dmxSocket, "dmxSocket", 4096, &dmxInterface, 5, NULL);
    xTaskCreate(dmx_Listener, "dmx_Listener", 4096, &dmxInterface, 5, NULL);

    Adc adcLight{adc_unit_t::ADC_UNIT_1, adc1_channel_t::ADC1_CHANNEL_0};
    Adc adcAmbiente{adc_unit_t::ADC_UNIT_1, adc1_channel_t::ADC1_CHANNEL_3};

    ScaledValue intensityScale{ScaledValue::Range{0,4095}, ScaledValue::Range{0,255}};
    
    {
        uint8_t data[dmxInterface.Size];
        for (size_t i = 0; i < dmxInterface.Size; i++) {
            data[i] = i + 1;
        }
        dmxInterface.set(data);
    }

    while (true) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        dmxInterface.send();
        intensities[0] = intensityScale.scale(adcLight.readValue());
        intensities[1] = intensityScale.scale(adcAmbiente.readValue());
    }
}
