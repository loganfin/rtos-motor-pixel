#include "pixel.h"
#include "seg_display.h"

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

enum pixel_rainbow_frames {
    PIXEL_RED    = 0,
    PIXEL_ORANGE = 1,
    PIXEL_YELLOW = 2,
    PIXEL_GREEN  = 3,
    PIXEL_BLUE   = 4,
    PIXEL_INDIGO = 5,
    PIXEL_VIOLET = 6,
    PIXEL_MAX    = 7,
};

void vPixel(void* parameters)
{
    uint color = PIXEL_RED;
    int8_t dir = 0;
    uint8_t start = 0;
    uint8_t end = 0;
    uint index = 0;
    uint rx_data = 0;
    uint new_control_data = 0;
    uint old_control_data = 0;

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
                        if (index == PIXEL_MAX) {
                            index = PIXEL_RED;
                        }
                        ledStrip.setPixelColor(0, color);
                    }
                    ledStrip.show();
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }
                break;
            case 'I':
                ledStrip.setPixelColor(0, pixel_rainbow[PIXEL_RED]);
                ledStrip.setPixelColor(1, pixel_rainbow[PIXEL_YELLOW]);
                ledStrip.setPixelColor(2, pixel_rainbow[PIXEL_GREEN]);
                ledStrip.setPixelColor(3, pixel_rainbow[PIXEL_VIOLET]);
                ledStrip.show();
                vTaskDelay(50 / portTICK_PERIOD_MS);
                break;
            case 'M':
                xQueuePeek(xQControl, &new_control_data, 0);
                if (new_control_data < old_control_data) {
                    for (int i = 0; i < pixel_strip_length; i++) {
                        ledStrip.setPixelColor(i, pixel_rainbow[PIXEL_VIOLET]);
                        if (i > 0) {
                            ledStrip.setPixelColor(i - 1, 0);
                        }
                        ledStrip.show();
                        vTaskDelay(100 / portTICK_PERIOD_MS);
                    }
                } else if (new_control_data > old_control_data) {
                    for (int i = pixel_strip_length - 1; i > -1; i--) {
                        ledStrip.setPixelColor(i, pixel_rainbow[PIXEL_VIOLET]);
                        if (i < pixel_strip_length - 1) {
                            ledStrip.setPixelColor(i + 1, 0);
                        }
                        ledStrip.show();
                        vTaskDelay(100 / portTICK_PERIOD_MS);
                    }
                } else {
                    for (int i = 0; i < pixel_strip_length; i++) {
                        ledStrip.setPixelColor(i, 0);
                        ledStrip.show();
                    }
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                }
                old_control_data = new_control_data;
                break;
            default:
                break;
        }
    }
}
