/*
 * This is the implementation of the ISR attached to the buttons along
 * with the tasks that are triggered by the ISR.
 */
#include "buttons.h"
#include "hdc.h"
#include "pixel.h"
#include "seg_display.h"
#include "stepper.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <pico/stdlib.h>
#include <stdio.h>

const uint buttons_s1_pin = 19;
const uint buttons_s2_pin = 9;
const uint buttons_s3_pin = 8;
const uint buttons_debounce_delay = 200 / portTICK_PERIOD_MS;

SemaphoreHandle_t xButton1Semaphore;
SemaphoreHandle_t xButton2Semaphore;
SemaphoreHandle_t xButton3Semaphore;

void buttons_init()
{
    gpio_init(buttons_s1_pin);
    gpio_init(buttons_s2_pin);
    gpio_init(buttons_s3_pin);

    gpio_set_dir(buttons_s1_pin, GPIO_IN);
    gpio_set_dir(buttons_s2_pin, GPIO_IN);
    gpio_set_dir(buttons_s3_pin, GPIO_IN);

    gpio_set_irq_enabled_with_callback(buttons_s1_pin, GPIO_IRQ_EDGE_RISE, 1, isr_buttons);
    gpio_set_irq_enabled(buttons_s2_pin, GPIO_IRQ_EDGE_RISE, 1);
    gpio_set_irq_enabled(buttons_s3_pin, GPIO_IRQ_EDGE_RISE, 1);
}

void isr_buttons(uint gpio, uint32_t events)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    switch (gpio) {
        case buttons_s1_pin:
            xSemaphoreGiveFromISR(xButton1Semaphore, &xHigherPriorityTaskWoken);
            break;
        case buttons_s2_pin:
            xSemaphoreGiveFromISR(xButton2Semaphore, &xHigherPriorityTaskWoken);
            break;
        case buttons_s3_pin:
            xSemaphoreGiveFromISR(xButton3Semaphore, &xHigherPriorityTaskWoken);
            break;
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* FreeRTOS Tasks */
void vButton1(void* parameters)
{
    uint count = 0;
    TickType_t input_frame = 2000 / portTICK_PERIOD_MS;
    TickType_t start = 0;
    TickType_t end = 0;
    display_packet tx_packet;
    uint tx_data;
    xButton1Semaphore = xSemaphoreCreateBinary();

    while(true) {
        if (start != 0) {
            end = xTaskGetTickCount() - start;
            if (xSemaphoreTake(xButton1Semaphore, 1) == pdTRUE) {
                // debounce
                vTaskDelay(buttons_debounce_delay);
                xSemaphoreTake(xButton1Semaphore, 0);

                // check the current count
                if (count == 0) {
                    start = xTaskGetTickCount();
                }
                count++;
            }
        }
        else if (start == 0) {
            if (xSemaphoreTake(xButton1Semaphore, portMAX_DELAY) == pdTRUE) {
                // debounce
                vTaskDelay(buttons_debounce_delay);
                xSemaphoreTake(xButton1Semaphore, 0);

                // check the current count
                if (count == 0) {
                    start = xTaskGetTickCount();
                }
                count++;
            }
        }

        // maybe move this above the button handler?
        tx_packet.duration = 1000 / portTICK_PERIOD_MS;
        if (end > input_frame) {
            switch (count) {
                case 1:
                    // push 'H' onto xQHDC to tell HDC1080 to read and transmit humidity
                    // motor should move on humidity
                    tx_data = 'h';
                    xQueueSend(xQHDCMotor, &tx_data, 0);
                    break;
                case 2:
                    // push 'M' onot xQMotor to tell motor to move depending on the peeed value in xQControl
                    // motor should move on temperature
                    tx_data = 't';
                    xQueueSend(xQHDCMotor, &tx_data, 0);
                    break;
                case 3:
                    // display "EE"
                    tx_packet.data = 0x0100;
                    xQueueSendToFront(xQControl, &tx_packet, 0);
                    break;
                case 4:
                    // toggle display between hex and decimal
                    tx_packet.data = 0x0200;
                    xQueueSendToFront(xQControl, &tx_packet, 0);
                    break;
                case 5:
                    // display rainbow animation on the pixel display
                    tx_data = 'R';
                    xQueueSend(xQPixel, &tx_data, 0);
                    break;
                default:
                    break;
            }
            start = 0;
            end = 0;
            count = 0;
        }
    }
}

void vButton2(void* parameters)
{
    uint count = 0;
    TickType_t input_frame = 2000 / portTICK_PERIOD_MS;
    TickType_t start = 0;
    TickType_t end = 0;
    display_packet tx_packet;
    int tx_data;
    xButton2Semaphore = xSemaphoreCreateBinary();

    while(true) {
        if (start != 0) {
            end = xTaskGetTickCount() - start;
            if (xSemaphoreTake(xButton2Semaphore, 1) == pdTRUE) {
                // debounce
                vTaskDelay(buttons_debounce_delay);
                xSemaphoreTake(xButton2Semaphore, 0);

                // check the current count
                if (count == 0) {
                    start = xTaskGetTickCount();
                }
                count++;
            }
        }
        else if (start == 0) {
            if (xSemaphoreTake(xButton2Semaphore, portMAX_DELAY) == pdTRUE) {
                // debounce
                vTaskDelay(buttons_debounce_delay);
                xSemaphoreTake(xButton2Semaphore, 0);

                // check the current count
                if (count == 0) {
                    start = xTaskGetTickCount();
                }
                count++;
            }
        }

        // maybe move this above the button handler?
        if (end > input_frame) {
            switch (count) {
                case 1:
                    // push 1 onto xQMotor to move counter-clockwise
                    tx_data = 1;
                    break;
                case 2:
                    // push -1 onto xQMotor to move counter-clockwise
                    tx_data = -1;
                    break;
                case 3:
                    // push 'A' onto xQMotor to alternate between clockwise and counter-clockwise
                    tx_data = 'A';
                    break;
                case 4:
                    // display unique colors on each pixel led
                    tx_data = 'I';
                    xQueueSend(xQPixel, &tx_data, 0);
                    break;
                default:
                    break;
            }
            start = 0;
            end = 0;
            count = 0;
            xQueueSend(xQHDCMotor, &count, 0);
            xQueueSend(xQMotor, &tx_data, 0);
        }
    }
}

void vButton3(void* parameters)
{
    uint count = 0;
    TickType_t input_frame = 2000 / portTICK_PERIOD_MS;
    TickType_t start = 0;
    TickType_t end = 0;
    display_packet tx_packet;
    uint tx_data;
    xButton3Semaphore = xSemaphoreCreateBinary();

    while(true) {
        if (start != 0) {
            end = xTaskGetTickCount() - start;
            if (xSemaphoreTake(xButton3Semaphore, 1) == pdTRUE) {
                // debounce
                vTaskDelay(buttons_debounce_delay);
                xSemaphoreTake(xButton3Semaphore, 0);

                // check the current count
                if (count == 0) {
                    start = xTaskGetTickCount();
                }
                count++;
            }
        }
        else if (start == 0) {
            if (xSemaphoreTake(xButton3Semaphore, portMAX_DELAY) == pdTRUE) {
                // debounce
                vTaskDelay(buttons_debounce_delay);
                xSemaphoreTake(xButton3Semaphore, 0);

                // check the current count
                if (count == 0) {
                    start = xTaskGetTickCount();
                }
                count++;
            }
        }

        // maybe move this above the button handler?
        if (end > input_frame) {
            switch (count) {
                case 1:
                    // push 'T' onto xQHDC to tell sensor to transmit temperature
                    // display temperature
                    tx_data = 'S';
                    xQueueSend(xQMotor, &tx_data, 0);
                    tx_data = 'T';
                    xQueueSend(xQHDC, &tx_data, 0);
                    break;
                case 2:
                    // push 'H' onto xQHDC to tell sensor to transmit humidity
                    // display humidity
                    tx_data = 'S';
                    xQueueSend(xQMotor, &tx_data, 0);
                    tx_data = 'H';
                    xQueueSend(xQHDC, &tx_data, 0);
                    break;
                case 3:
                    // push 'T' onto xQMotor to tell motor to transmit status on xQControl
                    // push 'M' onto xQDispMode to tell vDisplaymanager to show animations
                    tx_data = 'S';
                    xQueueSend(xQHDC, &tx_data, 0);
                    tx_data = 'M';
                    xQueueSend(xQMotor, &tx_data, 0);
                    break;
                case 4:
                    // display the change in value of the control queue
                    tx_data = 'M';
                    xQueueSend(xQPixel, &tx_data, 0);
                    break;
                default:
                    break;
            }
            start = 0;
            end = 0;
            count = 0;
        }
    }
}
