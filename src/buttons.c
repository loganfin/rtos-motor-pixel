/*
 * This is the implementation of the ISR attached to the buttons along
 * with the tasks that are triggered by the ISR.
 */
#include "buttons.h"
#include "FreeRTOSConfig.h"

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
    vSemaphoreCreateBinary(xButton1Semaphore);

    while(true) {
        // debounce
        // if the first press, start the timer
        // increment count
        // if end < time, try and take the semaphore again
        // else if end > time, switch statement to handlers

        // if the timer is started, don't block on semaphore

        //  if the timer is started:
        //      try to take the semaphore without blocking
        //  if the timer is not started:
        //      block on semaphore


        if (start != 0) {
            end = xTaskGetTickCount() - start;
            if (xSemaphoreTake(xButton1Semaphore, 0) == pdTRUE) {
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
            start = 0;
            end = 0;

            switch (count) {
                case 1:
                    printf("case 1 press. actual press %d\n", count);
                    break;
                case 2:
                    printf("case 2 press. actual press %d\n", count);
                    break;
                case 3:
                    printf("case 3 press. actual press %d\n", count);
                    break;
                case 4:
                    printf("case 4 press. actual press %d\n", count);
                    break;
                default:
                    printf("case default press. actual press %d\n", count);
                    break;
            }
            count = 0;
        }
    }
}

void vButton2()
{
    uint count = 0;
    TickType_t input_frame = 2 * configTICK_RATE_HZ;
    TickType_t start = 0;
    TickType_t end = 0;
    vSemaphoreCreateBinary(xButton2Semaphore);

    while(true) {
        /*
         * On button press:
         * give semaphore
         *
         * increment count
         */
        if (xSemaphoreTake(xButton2Semaphore, portMAX_DELAY) == pdTRUE) {
            // debounce the button
            vTaskDelay(buttons_debounce_delay);
            xSemaphoreTake(xButton2Semaphore, 0);

            // count the number of button presses within 2 seconds
            // what happens if the button is pressed within 2 seconds of startup?
            end = xTaskGetTickCount() - start;
            if (end > input_frame) {
                start = xTaskGetTickCount();
                count = 0;
            }
            count++;

            printf("button s2 has been pressed %d times\n", count);
            printf("start tick: %d\n", start);
            printf("end tick: %d\n", end);
            printf("input_frame tick: %d\n\n", input_frame);
        }
    }
}

void vButton3()
{
    uint count = 0;
    TickType_t input_frame = 2 * configTICK_RATE_HZ;
    TickType_t start = 0;
    TickType_t end = 0;
    vSemaphoreCreateBinary(xButton3Semaphore);

    while(true) {
        if (xSemaphoreTake(xButton3Semaphore, portMAX_DELAY) == pdTRUE) {
            /*
               if (end > input_frame) {
               start = xTaskGetTickCount();
               end = start;
               count = 0;
               }
               */

            vTaskDelay(buttons_debounce_delay);
            xSemaphoreTake(xButton2Semaphore, 0);
            count++;

            end = xTaskGetTickCount() - start;
            if (end > input_frame) {
                start = xTaskGetTickCount();
                end = start;
                count = 0;
                continue;
            }

            printf("button s2 has been pressed %d times\n", count);
            printf("start tick: %d\n", start);
            printf("end tick: %d\n", end);
            printf("input_frame tick: %d\n\n", input_frame);
        }
    }
}
