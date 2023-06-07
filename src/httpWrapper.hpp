#pragma once

#include "esp_http_client.h"
#include "esp_log.h"


class Http {
  public:
    static esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
        switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD("HTTP", "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD("HTTP", "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD("HTTP", "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD("HTTP", "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key,
                     evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD("HTTP", "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD("HTTP", "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD("HTTP", "HTTP_EVENT_DISCONNECTED");
            break;
        }
        return ESP_OK;
    }
};

