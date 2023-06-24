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

#include <Artnet.h>
#include <lwip/netdb.h>
#include <sys/param.h>

#include "nvs.h"
#include "nvs_flash.h"
#include <sys/socket.h>

#include "Nextion.h"

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

OtaHandler ota("http://192.168.178.10:8070/dmx_bridge.bin");
static const char *TAG = "dmx-bridge";

void onCommand(EmbeddedCli *embeddedCli, CliCommand *command);
void startUpdate(EmbeddedCli *cli, char *args, void *context);
void getDmxLayout(EmbeddedCli *cli, char *args, void *context);
void getDmxChannel(EmbeddedCli *cli, char *args, void *context);
void setDmxChannel(EmbeddedCli *cli, char *args, void *context);
void onHello(EmbeddedCli *cli, char *args, void *context);

static const char *welcomeString = "\
** Smart-DMX-Bridge **\n\
* \n\
* \n\
";

void AppendCallbackToShell(Cli &shell) {
    shell.addBinding({"hello", "Print Welcome Stream", true, (void *)welcomeString, onHello});
    shell.addBinding({"update", "Start OTA-Update-Process", true, nullptr, startUpdate});
    shell.addBinding({"get-layout", "Get channel info", false, nullptr, getDmxLayout});
    shell.addBinding(
        {"set-ch", "'c v' Set DMX-Channel c to value v", false, nullptr, setDmxChannel});
    shell.addBinding(
        {"get-ch", "'c' Get DMX-Channel c (omit c for all)", false, nullptr, getDmxChannel});
}

static Uart dmxPort(2, GPIO_NUM_32, GPIO_NUM_33, Uart::BaudRate::_250000Bd, 128, Uart::StopBits::_2sb);

static void interfaceTask(void *arg) {
    static Uart mntPort(1, GPIO_NUM_36, GPIO_NUM_4, Uart::BaudRate::_115200Bd);
    Cli shell(mntPort, onCommand);

    AppendCallbackToShell(shell);
    while (1) {
        shell.process();
    }
}

void startUpdate(EmbeddedCli *cli, char *args, void *context) {
    ota.enableUpdateTask();
}

uint8_t dmxChannels[24];
void getDmxLayout(EmbeddedCli *cli, char *args, void *context) {
    auto &port = static_cast<Cli *>(cli->appContext)->_port;
    port.write("This will be the layout\n");
}

void getDmxChannel(EmbeddedCli *cli, char *args, void *context) {
    auto &port = static_cast<Cli *>(cli->appContext)->_port;
    port.write(std::string("get channel : ") + args + "\n");
}

void setDmxChannel(EmbeddedCli *cli, char *args, void *context) {
    auto &port = static_cast<Cli *>(cli->appContext)->_port;
    port.write(std::string("set channel : ") + args + "\n");
}
void onCommand(EmbeddedCli *embeddedCli, CliCommand *command) {
    ESP_LOGI(TAG, "Received command: %s", command->name);
    embeddedCliTokenizeArgs(command->args);
    for (int i = 1; i <= embeddedCliGetTokenCount(command->args); ++i) {
        ESP_LOGI(TAG, "    arg %d : %s", i, embeddedCliGetToken(command->args, i));
    }
}

void onHello(EmbeddedCli *cli, char *args, void *context) {
    ESP_LOGI(TAG, "Hello ");
    auto &port = static_cast<Cli *>(cli->appContext)->_port;
    port.write((const char *)context);
    if (embeddedCliGetTokenCount(args) == 0)
        ESP_LOGI(TAG, "%s", (const char *)context);
    else
        ESP_LOGI(TAG, "%s", embeddedCliGetToken(args, 1));
}

EtherPins_t etherPins = {
    .ethPhyAddr = GPIO_NUM_0,
    .ethPhyRst = GPIO_NUM_NC,
    .ethPhyMdc = GPIO_NUM_23,
    .ethPhyMdio = GPIO_NUM_18,
    .ethPhyPower = GPIO_NUM_12,
};

#define NR_OF_IP_ADDRESSES_TO_WAIT_FOR (2)

IpInfo deviceInfo;

static xSemaphoreHandle s_semph_get_ip_addrs =
    xSemaphoreCreateCounting(NR_OF_IP_ADDRESSES_TO_WAIT_FOR, 0);
void gotIpCallback(IpInfo deviceInfo) {
    xSemaphoreGive(s_semph_get_ip_addrs);
}

/**
 * @brief Checks the netif description if it contains specified prefix.
 * All netifs created withing common connect component are prefixed with the module TAG,
 * so it returns true if the specified netif is owned by this module
 */
static bool is_our_netif(const char *prefix, esp_netif_t *netif) {
    return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;
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
            Cli shell(session, onCommand);
            AppendCallbackToShell(shell);
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





void bScheme1Cb(void *ptr)
{
    // todo ERROR
}
void bScheme2Cb(void *ptr)
{
    // todo ERROR
}
void bScheme3Cb(void *ptr)
{
    // todo ERROR
}
void bSchemeCustomCb(void *ptr)
{
    // todo ERROR
}

void hCustomFgCb(void *ptr)
{
    // Update Display color and color
}
void hCustomBgCb(void *ptr)
{
    // Update Display color and color
}

uint8_t scaleColorDown(uint8_t val, uint8_t max) {
    return val*max/0xFF;
}

uint32_t calcNextionColor(uint8_t r, uint8_t g, uint8_t b) {
    // red = 63488 / F800
    // green = 2016 /  07E0
    // blue = 31 / 1F
    return scaleColorDown(b, 0x1F) 
        & (scaleColorDown(g, 0x3F) << 5) 
        & (scaleColorDown(r, 0x1F) << (5+6));
}

static void displayTask(void *arg) {
    static Uart nxtPort(1, GPIO_NUM_36, GPIO_NUM_4, Uart::BaudRate::_115200Bd);
    NexPage page0    = NexPage(0, 0, "splashScreen");
    NexPage page1    = NexPage(1, 0, "workingPage");
    NexPage page2    = NexPage(2, 0, "infoPage");

    NexButton bScheme1 = NexButton(1, 1, "bScheme1");
    NexButton bScheme2 = NexButton(1, 4, "bScheme2");
    NexButton bScheme3 = NexButton(1, 7, "bScheme3");
    NexButton bSchemeCustom = NexButton(1, 10, "bSchemeCustom");

    NexText tScheme1Fg = NexText(1, 2, "tScheme1Fg");
    NexText tScheme1Bg = NexText(1, 3, "tScheme1Bg");
    NexText tScheme2Fg = NexText(1, 5, "tScheme2Fg");
    NexText tScheme2Bg = NexText(1, 6, "tScheme2Bg");
    NexText tScheme3Fg = NexText(1, 8, "tScheme3Fg");
    NexText tScheme3Bg = NexText(1, 9, "tScheme3Bg");
    NexText tCustomFg = NexText(1, 16, "tCustomFg");
    NexText tCustomBg = NexText(1, 17, "tCustomBg");

    NexSlider hCustomFg = NexSlider(1, 1, "hCustomFg");
    NexSlider hCustomBg = NexSlider(1, 1, "hCustomBg");

    NexText tName = NexText(2, 3, "tName");
    NexText tVersion = NexText(2, 4, "tVersion");
    NexText tAddress = NexText(2, 6, "tAddress");
    NexText tInfo = NexText(2, 8, "tInfo");
    NexText tStatus = NexText(2, 10, "tStatus");

    NexTouch *nex_listen_list[] = 
    {
        &bScheme1,
        &bScheme2,
        &bScheme3,
        &bSchemeCustom,
        &hCustomFg,
        &hCustomBg,
        nullptr
    };

    nexInit(nxtPort);
    ESP_LOGI("DISP", "begin setup");
   
    bScheme1.attachPop(bScheme1Cb, &bScheme1);
    bScheme2.attachPop(bScheme2Cb, &bScheme2);
    bScheme3.attachPop(bScheme3Cb, &bScheme3);
    bSchemeCustom.attachPop(bSchemeCustomCb, &bSchemeCustom);
    hCustomFg.attachPop(hCustomFgCb, &hCustomFg);
    hCustomBg.attachPop(hCustomBgCb, &hCustomBg);

    tScheme1Fg.Set_background_color_bco(calcNextionColor(0xFF,0,0));
    tScheme2Fg.Set_background_color_bco(calcNextionColor(0,0xFF,0));
    tScheme3Fg.Set_background_color_bco(calcNextionColor(0,0,0xFF));

    tName.setText("DMX-Bridge");
    tVersion.setText("T 0.0.0");
    tAddress.setText("0.0.0.0");
    tInfo.setText("Device Info");
    tStatus.setText("Device Status");

    ESP_LOGI("DISP", "setup finished");

    vTaskDelay(pdMS_TO_TICKS(1000));
    page1.show();

    while (1) {
        nexLoop(nex_listen_list);
    }
}

void app_main(void) {
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initEthernetHardware(etherPins, gotIpCallback);
    // xTaskCreate(interfaceTask, "uart_interfaceTask", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
    xTaskCreate(displayTask, "displayTask", 4096, NULL, 10, NULL);

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
    static DmxInterface dmxInterface(dmxPort);
    xTaskCreate(dmxSocket, "dmxSocket", 4096, &dmxInterface, 5, NULL);
    xTaskCreate(dmx_Listener, "dmx_Listener", 4096, &dmxInterface, 5, NULL);

    {
        uint8_t data[dmxInterface.Size];
        for (size_t i = 0; i < dmxInterface.Size; i++) {
            data[i] = i + 1;
        }
        dmxInterface.set(data);
    }

    static int cnt;
    while (true) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        std::cout << "Cnt OTA" << cnt++ << "\n";
        dmxInterface.send();
    }
}
