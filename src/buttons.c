#include "buttons.h"

#include "pico/stdlib.h"
#include <stdio.h>

const uint buttons_s1_pin = 19;
const uint buttons_s2_pin = 9;
const uint buttons_s3_pin = 8;
const uint buttons_debounce_delay = 200;

void buttons_init()
{
    gpio_init(buttons_s1_pin);
    gpio_init(buttons_s2_pin);
    gpio_init(buttons_s3_pin);

    gpio_set_dir(buttons_s1_pin, GPIO_IN);
    gpio_set_dir(buttons_s2_pin, GPIO_IN);
    gpio_set_dir(buttons_s3_pin, GPIO_IN);

    // set up irq response
    gpio_set_irq_enabled_with_callback(buttons_s1_pin, GPIO_IRQ_EDGE_FALL, 1, isr_buttons);
    gpio_set_irq_enabled(buttons_s2_pin, GPIO_IRQ_EDGE_FALL, 1);
    gpio_set_irq_enabled(buttons_s3_pin, GPIO_IRQ_EDGE_FALL, 1);
}

void isr_buttons(uint gpio, uint32_t events)
{
    switch (gpio) {
        case buttons_s1_pin:
            printf("Interrupt occurred at pin %d with event %d\n", buttons_s1_pin, events);
            break;
        case buttons_s2_pin:
            printf("Interrupt occurred at pin %d with event %d\n", buttons_s2_pin, events);
            break;
        case buttons_s3_pin:
            printf("Interrupt occurred at pin %d with event %d\n", buttons_s3_pin, events);
            break;
    }
}
