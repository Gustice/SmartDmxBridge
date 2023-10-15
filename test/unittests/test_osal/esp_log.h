#pragma once

#define ESP_LOGE( tag, format, ... ) printf("Error: " format, ##__VA_ARGS__)
