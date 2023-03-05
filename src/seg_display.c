#include "seg_display.h"

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "pico/stdlib.h"

const uint seg_display_cc1_pin    = 11;
const uint seg_display_cc2_pin    = 10;
const uint seg_display_cc_assert  = 0;
const uint seg_display_a_pin      = 26;
const uint seg_display_b_pin      = 27;
const uint seg_display_c_pin      = 29;
const uint seg_display_d_pin      = 18;
const uint seg_display_e_pin      = 25;
const uint seg_display_f_pin      = 7;
const uint seg_display_g_pin      = 28;
const uint seg_display_dp_pin     = 24;
const uint seg_display_led_assert = 1;
const uint seg_display_left       = 0;
const uint seg_display_right      = 1;

QueueHandle_t xQControl;
QueueHandle_t xQLeftDisp;
QueueHandle_t xQRightDisp;
SemaphoreHandle_t xSemDisp;

/* Seven Segment Display Drivers */

void seg_display_init()
{
    gpio_init(seg_display_cc1_pin);
    gpio_init(seg_display_cc2_pin);
    gpio_init(seg_display_a_pin);
    gpio_init(seg_display_b_pin);
    gpio_init(seg_display_c_pin);
    gpio_init(seg_display_d_pin);
    gpio_init(seg_display_e_pin);
    gpio_init(seg_display_f_pin);
    gpio_init(seg_display_g_pin);
    gpio_init(seg_display_dp_pin);

    gpio_set_dir(seg_display_cc1_pin, GPIO_OUT);
    gpio_set_dir(seg_display_cc2_pin, GPIO_OUT);
    gpio_set_dir(seg_display_a_pin,   GPIO_OUT);
    gpio_set_dir(seg_display_b_pin,   GPIO_OUT);
    gpio_set_dir(seg_display_c_pin,   GPIO_OUT);
    gpio_set_dir(seg_display_d_pin,   GPIO_OUT);
    gpio_set_dir(seg_display_e_pin,   GPIO_OUT);
    gpio_set_dir(seg_display_f_pin,   GPIO_OUT);
    gpio_set_dir(seg_display_g_pin,   GPIO_OUT);
    gpio_set_dir(seg_display_dp_pin,  GPIO_OUT);
}

void seg_display_digit(uint display, uint8_t digit)
{
    gpio_put(seg_display_a_pin, !seg_display_led_assert);
    gpio_put(seg_display_b_pin, !seg_display_led_assert);
    gpio_put(seg_display_c_pin, !seg_display_led_assert);
    gpio_put(seg_display_d_pin, !seg_display_led_assert);
    gpio_put(seg_display_e_pin, !seg_display_led_assert);
    gpio_put(seg_display_f_pin, !seg_display_led_assert);
    gpio_put(seg_display_g_pin, !seg_display_led_assert);
    gpio_put(seg_display_dp_pin, !seg_display_led_assert);

    switch (display) {
        case seg_display_left:
            gpio_put(seg_display_cc2_pin, !seg_display_cc_assert);
            gpio_put(seg_display_cc1_pin, seg_display_cc_assert);
            break;
        case seg_display_right:
            gpio_put(seg_display_cc1_pin, !seg_display_cc_assert);
            gpio_put(seg_display_cc2_pin, seg_display_cc_assert);
            break;
        default:
            gpio_put(seg_display_cc1_pin, !seg_display_cc_assert);
            gpio_put(seg_display_cc2_pin, !seg_display_cc_assert);
            break;
    }

    for (uint8_t mask = 0x01; mask > 0; mask <<= 1) {
        switch (digit & mask) {
            case 0x01:
                gpio_put(seg_display_a_pin, seg_display_led_assert);
                break;
            case 0x02:
                gpio_put(seg_display_b_pin, seg_display_led_assert);
                break;
            case 0x04:
                gpio_put(seg_display_c_pin, seg_display_led_assert);
                break;
            case 0x08:
                gpio_put(seg_display_d_pin, seg_display_led_assert);
                break;
            case 0x10:
                gpio_put(seg_display_e_pin, seg_display_led_assert);
                break;
            case 0x20:
                gpio_put(seg_display_f_pin, seg_display_led_assert);
                break;
            case 0x40:
                gpio_put(seg_display_g_pin, seg_display_led_assert);
                break;
            case 0x80:
                gpio_put(seg_display_dp_pin, seg_display_led_assert);
                break;
            default:
                break;
        }
    }
}

/* FreeRTOS Tasks */

void vDisplayManager()
{
    uint8_t rx_data    = 0;
    uint8_t left_digit  = 0x3f; // display zero
    uint8_t right_digit = 0x3f; // display zero

    xQControl = xQueueCreate(11, sizeof(uint16_t));
    xQLeftDisp = xQueueCreate(1, sizeof(uint8_t));
    xQRightDisp = xQueueCreate(1, sizeof(uint8_t));
    xSemDisp = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemDisp);

    while(true) {
        // check if queue is full
        // receive the data
        // separate data into two digits
        // send left digit byte to xQLeftDisp
        xQueueSendToBack(xQLeftDisp, &left_digit, 0);
        // send right digit byte to xQRightDisp
        xQueueSendToBack(xQRightDisp, &right_digit, 0);
        taskYIELD();
    }
}

void vLeftDisplay()
{
    uint8_t digit = 0;

    while(true) {
        if (xSemaphoreTake(xSemDisp, 0) == pdTRUE) {
            xQueueReceive(xQLeftDisp, &digit, 0);
            seg_display_digit(seg_display_left, digit);
            xSemaphoreGive(xSemDisp);
        }
        taskYIELD();
    }
}

void vRightDisplay()
{
    uint8_t digit = 0;

    while(true) {
        if (xSemaphoreTake(xSemDisp, 0) == pdTRUE) {
            xQueueReceive(xQRightDisp, &digit, 0);
            seg_display_digit(seg_display_right, digit);
            xSemaphoreGive(xSemDisp);
        }
        taskYIELD();
    }
}
