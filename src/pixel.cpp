#include "pixel.h"

#include "WS2812.hpp"
#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include <cmath>
#include <stdio.h>

const uint pixel_led_pin = 20;
const uint pixel_strip_length = 4;

void pixel_init()
{
}

void vPixel(void* parameters)
{
    uint color = 0;
    int8_t dir = 0;
    uint8_t start = 0;
    uint8_t end = 0;

    WS2812 ledStrip(
        pixel_led_pin,
        pixel_strip_length,
        pio0,
        0,
        WS2812::FORMAT_WRGB
    );

    while (true) {
        color = rand();
        dir = (rand() & 1 ? 1 : -1);
        start = (dir > 0 ? 0 : pixel_strip_length);
        end = (dir > 0 ? pixel_strip_length : 0);

        printf("test\n");
        for (uint8_t ledIndex = start; ledIndex != end; ledIndex += dir) {
            ledStrip.setPixelColor(ledIndex, color);
            ledStrip.show();
        }
        vTaskDelay(100);
    }
}
