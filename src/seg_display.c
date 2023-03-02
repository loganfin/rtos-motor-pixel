#include "seg_display.h"

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
