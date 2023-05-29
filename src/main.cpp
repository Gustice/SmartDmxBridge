#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

static int cnt;
void app_main() {
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Tick %d\n", cnt++);
    }
}