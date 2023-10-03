#include "etherInit.hpp"
#include <iostream>
#include <string.h>

static GotIpHandler gotIpHandler = nullptr;

static const char *TAG = "ETH";
static esp_eth_mac_t *mac = nullptr;
static esp_eth_phy_t *phy = nullptr;
static esp_eth_handle_t eth_handle = nullptr;
static esp_eth_netif_glue_handle_t eth_glue = nullptr;

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

esp_netif_t *get_netif_from_desc(const char *desc) {
    esp_netif_t *netif = nullptr;
    while ((netif = esp_netif_next(netif)) != NULL) {
        if (strcmp(esp_netif_get_desc(netif), desc) == 0) {
            return netif;
        }
    }
    return netif;
}

static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                                 void *event_data) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    uint8_t mac_addr[6] = {0};
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);

    IpInfo deviceInfo{.address = IpAddress(ip_info->ip.addr),
                      .subnet = IpAddress(ip_info->netmask.addr),
                      .gateway = IpAddress(ip_info->gw.addr),
                      .macAddress{0}};
    std::copy(std::begin(mac_addr), std::end(mac_addr), deviceInfo.macAddress.begin());

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "MACADD %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1],
             mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    if (gotIpHandler) {
        gotIpHandler(deviceInfo);
    }
}

static void eth_stop(void) {
    esp_netif_t *eth_netif = get_netif_from_desc("eth");
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

void initEthernetHardware(EtherPins_t etherPins, GotIpHandler gotIpCb) {
    assert(gotIpCb != nullptr && "IpCallback must not be null");
    gotIpHandler = gotIpCb;
    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());

    // Create new default instance of esp-netif for Ethernet
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&cfg);

    // Init MAC and PHY configs to default
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = etherPins.ethPhyAddr;
    phy_config.reset_gpio_num = etherPins.ethPhyRst;
    gpio_pad_select_gpio(etherPins.ethPhyPower);
    gpio_set_direction(etherPins.ethPhyPower, GPIO_MODE_OUTPUT);
    gpio_set_level(etherPins.ethPhyPower, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    mac_config.smi_mdc_gpio_num = etherPins.ethPhyMdc;
    mac_config.smi_mdio_gpio_num = etherPins.ethPhyMdio;
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
}

bool is_our_netif(const char *prefix, esp_netif_t *netif) {
    return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;
}
