#include "stepper.h"

#include "pico/stdlib.h"

const uint stepper_coil_1_pin = 6;
const uint stepper_coil_2_pin = 0;
const uint stepper_coil_3_pin = 1;
const uint stepper_coil_4_pin = 20;

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
