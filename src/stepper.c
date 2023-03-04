/*
 * This is the implementation for the 28BYJ-48 stepper motor tasks and drivers.
 */

#include "stepper.h"

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"

const uint stepper_coil_1_pin = 6;
const uint stepper_coil_2_pin = 0;
const uint stepper_coil_3_pin = 1;
const uint stepper_coil_4_pin = 12;
const uint stepper_rev_steps  = 524;    // approximate

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
void stepper_step(uint steps, bool clockwise)
{
    TickType_t delay = 1;      // delay for one tick

    if (clockwise) {
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
        sleep_ms(delay);
        gpio_put(stepper_coil_4_pin, 0);
        sleep_ms(delay);
        gpio_put(stepper_coil_2_pin, 1);
        sleep_ms(delay);
        gpio_put(stepper_coil_1_pin, 0);
        sleep_ms(delay);
        gpio_put(stepper_coil_3_pin, 1);
        sleep_ms(delay);
        gpio_put(stepper_coil_2_pin, 0);
        sleep_ms(delay);
        gpio_put(stepper_coil_4_pin, 1);
        sleep_ms(delay);
        gpio_put(stepper_coil_3_pin, 0);
        sleep_ms(delay);
    }
}

// spin the motor one full revolution in either direction
void stepper_rev(bool clockwise)
{
    stepper_step(stepper_rev_steps, clockwise);
}

/* FreeRTOS Tasks */
void vMotor()
{
    while(true) {
        stepper_rev(true);
    }
}
