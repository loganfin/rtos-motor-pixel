#include "pixel.h"

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include <stdio.h>

void vPixel(void* parameters)
{
    while (true) {
        vTaskDelay(100);
        printf("test\n");
    }
}
