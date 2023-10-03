/**
 * @file otaUpdate.hpp
 * @author Gustice
 * @brief Update functionality
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include <string>
#include "httpWrapper.hpp"

extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

/* The interface name value can refer to if_desc in esp_netif_defaults.h */
static const char *bind_interface_name = "eth";

/**
 * @brief Class for Update handling
 * 
 */
class OtaHandler {
    constexpr static int HashLength = 32;

  public:
    /**
     * @brief Configuration structure
     */
    struct OtaParam {
        /// @brief Update URL
        std::string updateUrl;
    };

    /// @brief Constructor
    /// @param updateUrl default FTP/HTTP-server URL to update file 
    OtaHandler(std::string updateUrl) : _param{updateUrl} {}

    static void get_sha256_of_partitions(void) {
        uint8_t sha_256[HashLength] = {0};
        esp_partition_t partition;

        // get sha256 digest for bootloader
        partition.address = ESP_BOOTLOADER_OFFSET;
        partition.size = ESP_PARTITION_TABLE_OFFSET;
        partition.type = ESP_PARTITION_TYPE_APP;
        esp_partition_get_sha256(&partition, sha_256);
        print_sha256(sha_256, "SHA-256 for bootloader: ");

        // get sha256 digest for running partition
        esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
        print_sha256(sha_256, "SHA-256 for current firmware: ");
    }

    /// @brief Start Update service
    /// @param param FTP/HTTP-server URL to update file  
    void startUpdateService(OtaParam param) {
        _param = param;
        xTaskCreate(&updateTask, "updateTask", 8192, &_param, 5, NULL);
    }

  private:
    static void updateTask(void *pvParameter) {
        OtaParam * param =  (OtaParam *) pvParameter;
        ESP_LOGI("OTA", "Starting OTA-update-service");
#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF
        esp_netif_t *netif = get_example_netif_from_desc(bind_interface_name);
        if (netif == NULL) {
            ESP_LOGE("OTA", "Can't find netif from interface description");
            abort();
        }
        struct ifreq ifr;
        esp_netif_get_netif_impl_name(netif, ifr.ifr_name);
        ESP_LOGI("OTA", "Bind interface name is %s", ifr.ifr_name);
#endif
        esp_http_client_config_t config = {
            .url = param->updateUrl.c_str(),
            .cert_pem = (char *)server_cert_pem_start,
            .event_handler = Http::_http_event_handler,
            .keep_alive_enable = true,
#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF
            .if_name = &ifr,
#endif
        };

        // #ifdef CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK // todo: this bad
        config.skip_cert_common_name_check = true;

        esp_err_t ret = esp_https_ota(&config);
        if (ret == ESP_OK) {
            esp_restart();
        } else {
            ESP_LOGE("OTA", "Firmware upgrade failed");
        }
        while (1) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    static void print_sha256(const uint8_t *image_hash, const char *label) {
        char hash_print[HashLength * 2 + 1];
        hash_print[HashLength * 2] = 0;
        for (int i = 0; i < HashLength; ++i) {
            sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
        }
        ESP_LOGI("OTA", "%s %s", label, hash_print);
    }


    OtaParam _param;
    const std::string _firmwareUpdateUrl;
};