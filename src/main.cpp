#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "cliWrapper.hpp"
#include "dmxInterface.hpp"
#include "tcpSocket.hpp"
#include "udpSocket.hpp"
#include "uart.hpp"

#include "esp_system.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <Artnet.h>
#include <lwip/netdb.h>
#include <string.h>
#include <sys/param.h>

static const char *TAG = "dmx-bridge";

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

#define PORT 23
#define KEEPALIVE_IDLE 5     // int "TCP keep-alive idle time(s)"
#define KEEPALIVE_INTERVAL 5 // int "TCP keep-alive interval time(s)"
#define KEEPALIVE_COUNT 3    // int "TCP keep-alive packet retry send counts"

#define ECHO_TASK_STACK_SIZE 4098
static IpInfo deviceInfo;

// 164 bytes is minimum size for this params on Arduino Nano

void onCommand(EmbeddedCli *embeddedCli, CliCommand *command);

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
    shell.addBinding({"get-layout", "Get channel info", false, nullptr, getDmxLayout});
    shell.addBinding(
        {"set-ch", "'c v' Set DMX-Channel c to value v", false, nullptr, setDmxChannel});
    shell.addBinding(
        {"get-ch", "'c' Get DMX-Channel c (omit c for all)", false, nullptr, getDmxChannel});
}

static Uart dmxPort(2, GPIO_NUM_32, GPIO_NUM_33, Uart::BaudRate::_250000Bd, Uart::StopBits::_2sb);

static void interfaceTask(void *arg) {
    static Uart mntPort(1, GPIO_NUM_36, GPIO_NUM_4, Uart::BaudRate::_115200Bd);
    Cli shell(mntPort, onCommand);

    AppendCallbackToShell(shell);
    while (1) {
        shell.process();
    }
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

static int cnt;

#define CONFIG_EXAMPLE_ETH_PHY_ADDR 0
#define CONFIG_EXAMPLE_ETH_PHY_RST_GPIO -1
#define CONFIG_EXAMPLE_ETH_MDC_GPIO 23
#define CONFIG_EXAMPLE_ETH_MDIO_GPIO 18
#define PIN_PHY_POWER 12

static esp_eth_handle_t eth_handle = NULL;
static esp_eth_netif_glue_handle_t eth_glue = NULL;
static esp_eth_mac_t *mac = NULL;
static esp_eth_phy_t *phy = NULL;

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                              void *event_data) {
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        std::copy(std::begin(mac_addr), std::end(mac_addr), deviceInfo.macAddress.begin());
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1],
                 mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

#define NR_OF_IP_ADDRESSES_TO_WAIT_FOR (2)

static xSemaphoreHandle s_semph_get_ip_addrs =
    xSemaphoreCreateCounting(NR_OF_IP_ADDRESSES_TO_WAIT_FOR, 0);

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                                 void *event_data) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    deviceInfo.address = IpAddress(ip_info->ip.addr);
    deviceInfo.subnet = IpAddress(ip_info->netmask.addr);
    deviceInfo.gateway = IpAddress(ip_info->gw.addr);

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    xSemaphoreGive(s_semph_get_ip_addrs);
}

esp_netif_t *get_example_netif_from_desc(const char *desc) {
    esp_netif_t *netif = NULL;
    char *expected_desc;
    asprintf(&expected_desc, "%s: %s", TAG, desc);
    while ((netif = esp_netif_next(netif)) != NULL) {
        if (strcmp(esp_netif_get_desc(netif), expected_desc) == 0) {
            free(expected_desc);
            return netif;
        }
    }
    free(expected_desc);
    return netif;
}

static void eth_stop(void) {
    esp_netif_t *eth_netif = get_example_netif_from_desc("eth");
    ESP_ERROR_CHECK(
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler));
    ESP_ERROR_CHECK(esp_eth_stop(eth_handle));
    ESP_ERROR_CHECK(esp_eth_del_netif_glue(eth_glue));
    ESP_ERROR_CHECK(esp_eth_driver_uninstall(eth_handle));
    ESP_ERROR_CHECK(phy->del(phy));
    ESP_ERROR_CHECK(mac->del(mac));

    esp_netif_destroy(eth_netif);
    /* start Ethernet driver state machine */
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
    if (len > DMX_DEV_WIDTH)
        len = DMX_DEV_WIDTH;
    
    dmxInterface->set(data, size);
    dmxInterface->send();
}

static void dmx_Socket(void * args) {
    dmxInterface = (DmxInterface *) args;
    while (1) {
            struct sockaddr_in dest_addr_ip4 {
                .sin_len = sizeof(sockaddr_in),
                .sin_family = AF_INET,
                .sin_port = htons(arx::artnet::DEFAULT_PORT),
                .sin_addr {.s_addr = htonl(INADDR_ANY),}
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
    TcpSocket::Config config{
        .keepAlive = 1,
        .keepIdle = KEEPALIVE_IDLE,
        .keepInterval = KEEPALIVE_INTERVAL,
        .keepCount = KEEPALIVE_COUNT,
    };
    struct sockaddr_storage dest_addr;

    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(PORT);

    int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", AF_INET);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1) {

        ESP_LOGI(TAG, "Socket waiting for connect");
        TcpSocket socket(config, listen_sock);
        Cli shell(socket, onCommand);

        AppendCallbackToShell(shell);

        while (socket.isActive()) {
            shell.process();
        }
        ESP_LOGI(TAG, "Tearing down socket");
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

static void dmx_Listener(void *arg) {
    DmxInterface *dmx = (DmxInterface *)arg;
    while (1) {
        auto bytes = dmx->receive();

        ESP_LOGI(TAG, "DMX-Data received (%d bytes): %d %d %d %d .. %d", bytes.size(), bytes[0],
                 bytes[1], bytes[2], bytes[3], bytes.back());
    }
}

void app_main(void) {
    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create new default instance of esp-netif for Ethernet
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&cfg);

    // Init MAC and PHY configs to default
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;
    gpio_pad_select_gpio((gpio_num_t)PIN_PHY_POWER);
    gpio_set_direction((gpio_num_t)PIN_PHY_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)PIN_PHY_POWER, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    mac_config.smi_mdc_gpio_num = CONFIG_EXAMPLE_ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = CONFIG_EXAMPLE_ETH_MDIO_GPIO;
    mac = esp_eth_mac_new_esp32(&mac_config);
    phy = esp_eth_phy_new_lan87xx(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);

    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
    /* attach Ethernet driver to TCP/IP stack */
    eth_glue = esp_eth_new_netif_glue(eth_handle);
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, eth_glue));

    // Register user defined event handers
    ESP_ERROR_CHECK(
        esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(
        esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));
    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    ESP_ERROR_CHECK(esp_register_shutdown_handler(&eth_stop));

    // xTaskCreate(interfaceTask, "uart_interfaceTask", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);

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
    xTaskCreate(dmx_Socket, "dmx_Socket", 4096, &dmxInterface, 5, NULL);
    xTaskCreate(dmx_Listener, "dmx_Listener", 4096, &dmxInterface, 5, NULL);

    {
        uint8_t data[dmxInterface.Size];
        for (size_t i = 0; i < dmxInterface.Size; i++) {
            data[i] = i + 1;
        }
        dmxInterface.set(data);
    }

    while (true) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        std::cout << "Cnt " << cnt++ << "\n";
        dmxInterface.send();
    }

    // todo: Delete Task dmx-Listener
}
