/**
 * @file etherInit.hpp
 * @author Gustice
 * @brief IP utilities
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ip.hpp"

/**
 * @brief IP-configuration
 */
struct EtherPins_t {
    gpio_num_t ethPhyAddr;
    gpio_num_t ethPhyRst;
    gpio_num_t ethPhyMdc;
    gpio_num_t ethPhyMdio;
    gpio_num_t ethPhyPower;
};

/// @brief Got-IPcallback signature
using GotIpHandler = void (*)(IpInfo);
/// @brief Init IP-stack
/// @param etherPins configuration
/// @param IpInfo callback
void initEthernetHardware(EtherPins_t etherPins, void (*)(IpInfo));

/**
 * @brief Checks the netif description if it contains specified prefix.
 * @details All netifs created withing common connect component are prefixed with the module TAG,
 * so it returns true if the specified netif is owned by this module
/// @param prefix tag to search for
/// @param netif NETIF-descriptor
 */
bool is_our_netif(const char *prefix, esp_netif_t *netif);

/**
 * @brief Queries Net-IF descriptor by name
 * 
 * @param desc network name
 * @return esp_netif_t* descriptor
 */
esp_netif_t *get_netif_from_desc(const char *desc);

/**
 * @brief Create default IP-V4 Configuration
 * 
 * @param ip IP-word
 * @param port Port
 * @return sockaddr_in NET-IF compatible configuration
 */
sockaddr_in createIpV4Config(uint32_t ip, uint16_t port);
