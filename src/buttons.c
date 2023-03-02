#include "buttons.h"

#include "pico/stdlib.h"

const uint button_1_pin = 19;
const uint button_2_pin = 9;
const uint button_3_pin = 8;

void buttons_init()
{
    gpio_init(button_1_pin);
    gpio_init(button_2_pin);
    gpio_init(button_3_pin);

    gpio_set_dir(button_1_pin, GPIO_OUT);
    gpio_set_dir(button_2_pin, GPIO_OUT);
    gpio_set_dir(button_3_pin, GPIO_OUT);
}
