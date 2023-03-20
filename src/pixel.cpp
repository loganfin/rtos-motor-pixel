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

const uint pixel_rainbow[7] = {
    0x00000100, // red
    0x01000301, // orange
    0x01000101, // yellow
    0x01000001, // green
    0x00010000, // blue
    0x00020100, // indigo
    0x00010100, // violet
};

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
                for (int i = 1; i < pixel_strip_length; i++) {
                    ledStrip.setPixelColor(i, color);
                    if (i == pixel_strip_length - 1) {
                        color = pixel_rainbow[index];
                        index++;
                        if (index > 6) {
                            index = 0;
                        }
                        ledStrip.setPixelColor(0, color);
                    }
                    ledStrip.show();
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }
                break;
            case 'I':
                vTaskDelay(50 / portTICK_PERIOD_MS);
                break;
            case 'M':
                vTaskDelay(50 / portTICK_PERIOD_MS);
                break;
            default:
                break;
        }
    }
}
