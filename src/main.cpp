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

#include "embedded_cli.h"

static const char *TAG = "dmx-bridge";

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

#define ECHO_TEST_TXD 4
#define ECHO_TEST_RXD 36
#define ECHO_TEST_RTS -1
#define ECHO_TEST_CTS -1

#define ECHO_UART_PORT_NUM 1
#define ECHO_UART_BAUD_RATE 115200
#define ECHO_TASK_STACK_SIZE 4098

#define BUF_SIZE (1028)

// 164 bytes is minimum size for this params on Arduino Nano
#define CLI_BUFFER_SIZE 512
#define CLI_RX_BUFFER_SIZE 16
#define CLI_CMD_BUFFER_SIZE 32
#define CLI_HISTORY_SIZE 16
#define CLI_BINDING_COUNT 3

EmbeddedCli *cli;

CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];
void onCommand(EmbeddedCli *embeddedCli, CliCommand *command);
void writeChar(EmbeddedCli *embeddedCli, char c);
void onHello(EmbeddedCli *cli, char *args, void *context);
void onLed(EmbeddedCli *cli, char *args, void *context);
void onAdc(EmbeddedCli *cli, char *args, void *context);

static void echo_task(void *arg) {
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install((uart_port_t)ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL,
                                        intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS,
                                 ECHO_TEST_CTS));

    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
    config->cliBuffer = cliBuffer;
    config->cliBufferSize = CLI_BUFFER_SIZE;
    config->rxBufferSize = CLI_RX_BUFFER_SIZE;
    config->cmdBufferSize = CLI_CMD_BUFFER_SIZE;
    config->historyBufferSize = CLI_HISTORY_SIZE;
    config->maxBindingCount = CLI_BINDING_COUNT;
    cli = embeddedCliNew(config);

    if (cli == NULL) {
        ESP_LOGE(TAG, "Cli was not created, requires %d bytes. Check sizes!",
                 embeddedCliRequiredSize(config));
        return;
    }

    embeddedCliAddBinding(cli, {"get-led", "Get led status", false, nullptr, onLed});
    embeddedCliAddBinding(cli, {"get-adc", "Read adc value", false, nullptr, onAdc});
    embeddedCliAddBinding(cli, {"hello", "Print hello message", true, (void *)"World", onHello});

    cli->onCommand = onCommand;
    cli->writeChar = writeChar;

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1) {
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
        for (size_t i = 0; i < len; i++) {
            embeddedCliReceiveChar(cli, data[i]);
        }
        embeddedCliProcess(cli);
    }
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
    if (embeddedCliGetTokenCount(args) == 0)
        ESP_LOGI(TAG, "%s", (const char *)context);
    else
        ESP_LOGI(TAG, "%s", embeddedCliGetToken(args, 1));
}

void onLed(EmbeddedCli *cli, char *args, void *context) {
    ESP_LOGI(TAG, "LED: %li \r\n", random());
}

void onAdc(EmbeddedCli *cli, char *args, void *context) {
    ESP_LOGI(TAG, "ADC: %li \r\n", random());
}

void writeChar(EmbeddedCli *embeddedCli, char c) {
    uart_write_bytes(ECHO_UART_PORT_NUM, (const char *)&c, 1);
}

static int cnt;

#define CONFIG_EXAMPLE_ETH_PHY_ADDR 0
#define CONFIG_EXAMPLE_ETH_PHY_RST_GPIO -1
#define CONFIG_EXAMPLE_ETH_MDC_GPIO 23
#define CONFIG_EXAMPLE_ETH_MDIO_GPIO 18
#define PIN_PHY_POWER 12

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

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                                 void *event_data) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
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
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
    /* attach Ethernet driver to TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

    // Register user defined event handers
    ESP_ERROR_CHECK(
        esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(
        esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));
    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));

    xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);

    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        std::cout << "Cnt " << cnt++ << "\n";
    }
}
