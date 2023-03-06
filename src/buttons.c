/*
 * This is the implementation of the ISR attached to the buttons along
 * with the tasks that are triggered by the ISR.
 */
#include "buttons.h"
#include "seg_display.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <pico/stdlib.h>
#include <stdio.h>

const uint buttons_s1_pin = 19;
const uint buttons_s2_pin = 9;
const uint buttons_s3_pin = 8;
const uint buttons_debounce_delay = 160 / portTICK_PERIOD_MS;

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

    gpio_init(20);
    gpio_set_dir(20, GPIO_OUT);
    gpio_put(20, 1);
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
void vButton1()
{
    uint count = 0;
    TickType_t input_frame = 2 * configTICK_RATE_HZ;
    TickType_t start = 0;
    TickType_t end = 0;
    uint16_t control_queue_message = 0;
    vSemaphoreCreateBinary(xButton1Semaphore);

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
        if (end > input_frame) {
            switch (count) {
                case 1:
                    printf("case 1 press. actual press %d\n", count);
                    control_queue_message = 1;
                    break;
                case 2:
                    printf("case 2 press. actual press %d\n", count);
                    control_queue_message = 2;
                    break;
                case 3:
                    printf("case 3 press. actual press %d\n", count);
                    control_queue_message = 3;
                    break;
                case 4:
                    printf("case 4 press. actual press %d\n", count);
                    control_queue_message = 4;
                    break;
                default:
                    printf("case default press. actual press %d\n", count);
                    control_queue_message = 18;
                    break;
            }
            control_queue_message = count;
            start = 0;
            end = 0;
            count = 0;
            xQueueSendToBack(xQControl, &control_queue_message, 0);
        }
    }
}

void vButton2()
{
    uint count = 0;
    TickType_t input_frame = 2 * configTICK_RATE_HZ;
    TickType_t start = 0;
    TickType_t end = 0;
    uint16_t control_queue_message = 0;
    vSemaphoreCreateBinary(xButton2Semaphore);

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
                    printf("case 1 press. actual press %d\n", count);
                    control_queue_message = 10;
                    break;
                case 2:
                    printf("case 2 press. actual press %d\n", count);
                    control_queue_message = 20;
                    break;
                case 3:
                    printf("case 3 press. actual press %d\n", count);
                    control_queue_message = 30;
                    break;
                case 4:
                    printf("case 4 press. actual press %d\n", count);
                    control_queue_message = 40;
                    break;
                default:
                    printf("case default press. actual press %d\n", count);
                    control_queue_message = 50;
                    break;
            }
            start = 0;
            end = 0;
            count = 0;
            xQueueSendToBack(xQControl, &control_queue_message, 0);
        }
    }
}

void vButton3()
{
    uint count = 0;
    TickType_t input_frame = 2 * configTICK_RATE_HZ;
    TickType_t start = 0;
    TickType_t end = 0;
    uint16_t control_queue_message = 0;
    vSemaphoreCreateBinary(xButton3Semaphore);

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
                    printf("case 1 press. actual press %d\n", count);
                    control_queue_message = 11;
                    break;
                case 2:
                    printf("case 2 press. actual press %d\n", count);
                    control_queue_message = 22;
                    break;
                case 3:
                    printf("case 3 press. actual press %d\n", count);
                    control_queue_message = 33;
                    break;
                case 4:
                    printf("case 4 press. actual press %d\n", count);
                    control_queue_message = 44;
                    break;
                default:
                    printf("case default press. actual press %d\n", count);
                    control_queue_message = 55;
                    break;
            }
            start = 0;
            end = 0;
            count = 0;
            xQueueSendToBack(xQControl, &control_queue_message, 0);
        }
    }
}
