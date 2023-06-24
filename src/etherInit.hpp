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

struct EtherPins_t {
    gpio_num_t ethPhyAddr;
    gpio_num_t ethPhyRst;
    gpio_num_t ethPhyMdc;
    gpio_num_t ethPhyMdio;
    gpio_num_t ethPhyPower;
};

using GotIpHandler = void(*)(IpInfo);
void initEthernetHardware(EtherPins_t etherPins, void(*)(IpInfo));