#include "seg_display.h"

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "pico/stdlib.h"
#include <stdio.h>

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

enum {
    DISP_ZERO  = 0,
    DISP_ONE   = 1,
    DISP_TWO   = 2,
    DISP_THREE = 3,
    DISP_FOUR  = 4,
    DISP_FIVE  = 5,
    DISP_SIX   = 6,
    DISP_SEVEN = 7,
    DISP_EIGHT = 8,
    DISP_NINE  = 9,
    DISP_A     = 10,
    DISP_b     = 11,
    DISP_C     = 12,
    DISP_d     = 13,
    DISP_E     = 14,
    DISP_F     = 15,
    DISP_h     = 16,
    DISP_NUM   = 17,
};

uint8_t seg_display_num[DISP_NUM] = {
    0b00111111, // 0x3F 0
    0b00000110, // 0x06 1
    0b01011011, // 0x5B 2
    0b01001111, // 0x4F 3
    0b01100110, // 0x66 4
    0b01101101, // 0x6D 5
    0b01111101, // 0x7D 6
    0b00000111, // 0x07 7
    0b01111111, // 0x7F 8
    0b01100111, // 0x67 9
    0b01110111, // 0x77 A
    0b01111100, // 0x7C b
    0b00111001, // 0x39 C
    0b01011110, // 0xE5 d
    0b01111001, // 0x79 E
    0b01110001, // 0x71 F
    0b01110100, // 0x74 h
};

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

void seg_display_off()
{
    seg_display_digit(seg_display_left, 0);
    seg_display_digit(seg_display_right, 0);
}

/* FreeRTOS Tasks */

void vDisplayManager()
{
    uint num_base        = 10;
    uint16_t rx_data     = 0;
    uint8_t left_digit   = 0;
    uint8_t right_digit  = 0;
    uint8_t display_mode = 0;
    bool hexadecimal     = false;
    TickType_t duration  = 0;
    TickType_t start     = 0;
    TickType_t end       = 0;

    xQControl   = xQueueCreate(11, sizeof(uint16_t));
    xQLeftDisp  = xQueueCreate(1, sizeof(uint8_t));
    xQRightDisp = xQueueCreate(1, sizeof(uint8_t));
    xSemDisp    = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemDisp);

    while(true) {
        // check if queue is full
        end = xTaskGetTickCount() - start;
        if (end > duration) {
            //printf("end: %d\n", end);
            //printf("duration: %d\n", duration);
            left_digit  = 0;
            right_digit = 0;
            // BRILLIANT IDEA:
            // sending duration and then sending the content is TOO ERROR PRONE
            // top nibble of rx_data = duration
            // second nibble from top of rx_data = special characters (E, G, M, ...)
            // bottom byte = data
            if (uxQueueSpacesAvailable(xQControl) == 0) {
                left_digit = seg_display_num[DISP_ZERO];
                right_digit = seg_display_num[DISP_F];
                duration = 2 * configTICK_RATE_HZ;
                start = xTaskGetTickCount();
                end = start;
                xQueueReceive(xQControl, &rx_data, 1);
                xQueueReceive(xQControl, &rx_data, 1);
            }
            // receive the data
            else if (xQueueReceive(xQControl, &rx_data, 1)) {
                // get the duration time
                //xQueueReceive(xQControl, &duration, 1);
                duration = (rx_data >> 12) * configTICK_RATE_HZ;
                rx_data &= 0x0fff;
                printf("rx_data: %d\n", rx_data);
                printf("duration: %d\n", duration);
                start = xTaskGetTickCount();
                end = start;
                // either a number or a stepper motor status
                // perhaps stepper motor should have msb set
                // 1000 0000 0000 0000 = clockwise
                // 1100 0000 0000 0000 = counter clockwise
                //
                // rx_data = 98;
                // left_digit = seg_display_9_digit;
                // right_digit = seg_display_8_digit;
                //
                // 9 = seg_display_9_digit;
                // left_digit = seg_display_num[seg_display_9_digit];
                //
                // left_digit = seg_display_num[rx_data >> 8];
                // right_digit = seg_display_num[rx_data & 0x00FF];
                //
                // separate data into two digits
                switch (rx_data) {
                    case 0x0100: // E
                        left_digit = seg_display_num[DISP_E];
                        right_digit = left_digit;
                        duration = 4 * duration;
                        break;
                    case 0x0200: // g
                        hexadecimal = !hexadecimal;
                        if (hexadecimal) {
                            left_digit = seg_display_num[DISP_h];
                            right_digit = left_digit;
                            num_base = 16;
                        }
                        else {
                            left_digit = seg_display_num[DISP_d];
                            right_digit = left_digit;
                            num_base = 10;
                        }
                    case 'M':
                        break;
                    default:
                        left_digit = seg_display_num[rx_data / num_base];
                        right_digit = seg_display_num[rx_data % num_base];
                        break;
                }
                printf("rx_data: %d\n", rx_data);
            }

            // send left digit byte to xQLeftDisp
            xQueueSendToBack(xQLeftDisp, &left_digit, 0);
            // send right digit byte to xQRightDisp
            xQueueSendToBack(xQRightDisp, &right_digit, 0);
        }
        else {
        }
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
        //printf("Left\n");
        //vTaskDelay(1 * configTICK_RATE_HZ);
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
        //printf("Right\n");
        //vTaskDelay(1 * configTICK_RATE_HZ);
        taskYIELD();
    }
}
