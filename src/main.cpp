#include "etherInit.hpp"

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
#include "httpWrapper.hpp"

#include <Artnet.h>
#include <lwip/netdb.h>
#include <sys/param.h>

#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <sys/socket.h>


#define HASH_LEN 32
/* The interface name value can refer to if_desc in esp_netif_defaults.h */
static const char *bind_interface_name = "eth";

extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

#define CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK // todo Bad ... remove
#define CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL "http://192.168.178.10:8070/hello_world.bin"

#define OTA_URL_SIZE 256

static const char *TAG = "dmx-bridge";

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

#define PORT 23
#define KEEPALIVE_IDLE 5     // int "TCP keep-alive idle time(s)"
#define KEEPALIVE_INTERVAL 5 // int "TCP keep-alive interval time(s)"
#define KEEPALIVE_COUNT 3    // int "TCP keep-alive packet retry send counts"

#define ECHO_TASK_STACK_SIZE 4098

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

static Uart dmxPort(2, GPIO_NUM_32, GPIO_NUM_33, Uart::BaudRate::_250000Bd, 128, Uart::StopBits::_2sb);

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
    TcpSession::Config config{
        .keepAlive = 1,
        .keepIdle = KEEPALIVE_IDLE,
        .keepInterval = KEEPALIVE_INTERVAL,
        .keepCount = KEEPALIVE_COUNT,
    };
    struct sockaddr_in dest_addr_ip4 {
        .sin_len = sizeof(sockaddr_in), .sin_family = AF_INET, .sin_port = htons(PORT),
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




void simple_ota_example_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA example");
#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF
    esp_netif_t *netif = get_example_netif_from_desc(bind_interface_name);
    if (netif == NULL) {
        ESP_LOGE(TAG, "Can't find netif from interface description");
        abort();
    }
    struct ifreq ifr;
    esp_netif_get_netif_impl_name(netif, ifr.ifr_name);
    ESP_LOGI(TAG, "Bind interface name is %s", ifr.ifr_name);
#endif
    esp_http_client_config_t config = {
        .url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
        .cert_pem = (char *)server_cert_pem_start,
        .event_handler = Http::_http_event_handler,
        .keep_alive_enable = true,
#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF
        .if_name = &ifr,
#endif
    };

#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL_FROM_STDIN
    char url_buf[OTA_URL_SIZE];
    if (strcmp(config.url, "FROM_STDIN") == 0) {
        example_configure_stdin_stdout();
        fgets(url_buf, OTA_URL_SIZE, stdin);
        int len = strlen(url_buf);
        url_buf[len - 1] = '\0';
        config.url = url_buf;
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong firmware upgrade image url");
        abort();
    }
#endif

#ifdef CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK
    config.skip_cert_common_name_check = true;
#endif

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void print_sha256(const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s %s", label, hash_print);
}

static void get_sha256_of_partitions(void)
{
    uint8_t sha_256[HASH_LEN] = { 0 };
    esp_partition_t partition;

    // get sha256 digest for bootloader
    partition.address   = ESP_BOOTLOADER_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_OFFSET;
    partition.type      = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");
}







void app_main(void) {
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initEthernetHardware(etherPins, gotIpCallback);
    // xTaskCreate(interfaceTask, "uart_interfaceTask", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);

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

    get_sha256_of_partitions();


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

    // xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);

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

    while (true) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        std::cout << "Cnt " << cnt++ << "\n";
        dmxInterface.send();
    }
}
