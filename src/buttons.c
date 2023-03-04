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
const uint buttons_debounce_delay = 110 / portTICK_PERIOD_MS;

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
void vButton1()
{
    uint count = 0;
    vSemaphoreCreateBinary(xButton1Semaphore);

    while(true) {
        xSemaphoreTake(xButton1Semaphore, portMAX_DELAY);
        vTaskDelay(buttons_debounce_delay);
        xSemaphoreTake(xButton1Semaphore, 0);
        printf("button s1 has been pressed %d times\n", count);
        count++;
    }
}

void vButton2()
{
    uint count = 0;
    vSemaphoreCreateBinary(xButton2Semaphore);

    while(true) {
        xSemaphoreTake(xButton2Semaphore, portMAX_DELAY);
        vTaskDelay(buttons_debounce_delay);
        xSemaphoreTake(xButton2Semaphore, 0);
        printf("button s2 has been pressed %d times\n", count);
        count++;
    }
}

void vButton3()
{
    uint count = 0;
    vSemaphoreCreateBinary(xButton3Semaphore);

    while(true) {
        xSemaphoreTake(xButton3Semaphore, portMAX_DELAY);
        vTaskDelay(buttons_debounce_delay);
        xSemaphoreTake(xButton3Semaphore, 0);
        printf("button s3 has been pressed %d times\n", count);
        count++;
    }
}
