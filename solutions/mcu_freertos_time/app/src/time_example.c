#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

void example_main(void)
{
    uint64_t uwTickCount = 0;
    uint32_t cnt = 10;

    uwTickCount = xTaskGetTickCount();
    printf("xTaskGetTickCount = %u \n", (uint32_t)uwTickCount);

    printf("print cnt every 1s for %u times\n", cnt);

    while (cnt--) {
        uwTickCount += 100;
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("-----%u\n", cnt);
    }

    cnt = 3;
    printf("print cnt every 3s for %u times\n", cnt);

    while (cnt--) {
        uwTickCount += 300;
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("-----%u\n", cnt);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
    uwTickCount = xTaskGetTickCount();
    printf("xTaskGetTickCount = %u \n", (uint32_t)uwTickCount);

    vTaskDelay(pdMS_TO_TICKS(1000));
    uwTickCount = xTaskGetTickCount();
    printf("xTaskGetTickCount after delay = %u \n", (uint32_t)uwTickCount);

    printf("test kernel time successfully!\n");
    vTaskDelete(NULL);
}

