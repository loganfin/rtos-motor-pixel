#include "pixel.h"

#include "WS2812.hpp"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "pico/stdlib.h"
#include <cmath>
#include <stdio.h>

const uint pixel_led_pin = 20;
const uint pixel_strip_length = 4;

QueueHandle_t xQPixel;

void vPixel(void* parameters)
{
    uint color = 0;
    int8_t dir = 0;
    uint8_t start = 0;
    uint8_t end = 0;
    uint index = 0;
    uint rx_data = 0;

    WS2812 ledStrip(pixel_led_pin, pixel_strip_length,
            pio0, 0, WS2812::FORMAT_WRGB);

    while (true) {
        xQueueReceive(xQPixel, &rx_data, 0);
        switch (rx_data) {
            case 'R':
                printf("print rainbow\n");
                vTaskDelay(50 / portTICK_PERIOD_MS);
                break;
            case 'I':
                printf("print individual colors\n");
                vTaskDelay(50 / portTICK_PERIOD_MS);
                break;
            case 'M':
                printf("print status of motor\n");
                vTaskDelay(50 / portTICK_PERIOD_MS);
                break;
            default:
                break;
        }
    }
}
