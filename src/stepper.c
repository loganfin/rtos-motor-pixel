/*
 * This is the implementation for the 28BYJ-48 stepper motor tasks and drivers.
 */

#include "stepper.h"
#include "seg_display.h"

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "pico/stdlib.h"
#include <stdio.h>

const uint stepper_coil_1_pin = 6;
const uint stepper_coil_2_pin = 0;
const uint stepper_coil_3_pin = 1;
const uint stepper_coil_4_pin = 12;
const uint stepper_rev_steps  = 524;    // approximate
const uint stepper_clockwise          = 1;

QueueHandle_t xQMotor;
QueueHandle_t xQMotorData;

/* Stepper Motor Drivers */
// initialize all of the necessary pins
void stepper_init()
{
    gpio_init(stepper_coil_1_pin);
    gpio_init(stepper_coil_2_pin);
    gpio_init(stepper_coil_3_pin);
    gpio_init(stepper_coil_4_pin);

    gpio_set_dir(stepper_coil_1_pin, GPIO_OUT);
    gpio_set_dir(stepper_coil_2_pin, GPIO_OUT);
    gpio_set_dir(stepper_coil_3_pin, GPIO_OUT);
    gpio_set_dir(stepper_coil_4_pin, GPIO_OUT);
}

// spin the motor one step per call in the specified direction
void stepper_step(uint steps, uint direction)
{
    TickType_t delay = 1;      // delay for one tick

    if (direction == stepper_clockwise) {
        for (int i = 0; i < steps; i++) {
            gpio_put(stepper_coil_4_pin, 1);
            vTaskDelay(delay);
            gpio_put(stepper_coil_1_pin, 0);
            vTaskDelay(delay);
            gpio_put(stepper_coil_3_pin, 1);
            vTaskDelay(delay);
            gpio_put(stepper_coil_4_pin, 0);
            vTaskDelay(delay);
            gpio_put(stepper_coil_2_pin, 1);
            vTaskDelay(delay);
            gpio_put(stepper_coil_3_pin, 0);
            vTaskDelay(delay);
            gpio_put(stepper_coil_1_pin, 1);
            vTaskDelay(delay);
            gpio_put(stepper_coil_2_pin, 0);
            vTaskDelay(delay);
        }
        return;
    }

    for (int i = 0; i < steps; i++) {
        gpio_put(stepper_coil_1_pin, 1);
        vTaskDelay(delay);
        gpio_put(stepper_coil_4_pin, 0);
        vTaskDelay(delay);
        gpio_put(stepper_coil_2_pin, 1);
        vTaskDelay(delay);
        gpio_put(stepper_coil_1_pin, 0);
        vTaskDelay(delay);
        gpio_put(stepper_coil_3_pin, 1);
        vTaskDelay(delay);
        gpio_put(stepper_coil_2_pin, 0);
        vTaskDelay(delay);
        gpio_put(stepper_coil_4_pin, 1);
        vTaskDelay(delay);
        gpio_put(stepper_coil_3_pin, 0);
        vTaskDelay(delay);
    }
}

/* FreeRTOS Tasks */
void vMotor()
{
    int rx_data = 0;
    int last_status;
    uint direction = stepper_clockwise;
    uint step_count = 0;
    int addend = 1;
    bool send_status = false;
    display_packet status_packet;
    TickType_t end = 0;

    status_packet.duration = 100;
    xQMotor = xQueueCreate(1, sizeof(int));

    while(true) {
        xQueueReceive(xQMotor, &rx_data, 1);
        if (last_status != rx_data && send_status == true) {
            xQueueReset(xQControl);
            printf("queue cleared 1\n");
            printf("last status: %d\n", last_status);
            printf("rx_data: %d\n", rx_data);
        }

        switch (rx_data) {
            case 0:
                last_status = rx_data;
                status_packet.data = 0x0300;
                break;
            case 1:
                stepper_step(1, stepper_clockwise);
                last_status = rx_data;
                status_packet.data = stepper_clockwise;
                break;
            case -1:
                stepper_step(1, !stepper_clockwise);
                last_status = rx_data;
                status_packet.data = !stepper_clockwise;
                break;
            case 'A':
                last_status = rx_data;
                if (step_count == 0) {
                    addend = 1;
                    if (send_status == true) {
                        xQueueReset(xQControl);
                        printf("queue cleared 2\n");
                    }
                }
                else if (step_count == stepper_rev_steps) {
                    addend = -1;
                    if (send_status == true) {
                        xQueueReset(xQControl);
                        printf("queue cleared 3\n");
                    }
                }
                step_count += addend;

                if (addend == 1) {
                    stepper_step(1, stepper_clockwise);
                    status_packet.data = stepper_clockwise;
                }
                else if (addend == -1) {
                    stepper_step(1, !stepper_clockwise);
                    status_packet.data = !stepper_clockwise;
                }
                break;
            case 'M':
                rx_data = last_status;
                send_status = true;
                break;
            case 'S':
                rx_data = last_status;
                send_status = false;
                break;
            default:
                taskYIELD();
                break;
        }
        if (send_status == true && xTaskGetTickCount() > end) {
            end = xTaskGetTickCount() + (600 / portTICK_PERIOD_MS);
            printf("sending %d\n", status_packet.data);
            xQueueSend(xQControl, &status_packet, 0);
        }
    }
}
