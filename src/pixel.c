#include "pixel.h"

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include <stdio.h>

void vPixel()
{
    while (true) {
        vTaskDelay(100);
        printf("test\n");
    }
}
