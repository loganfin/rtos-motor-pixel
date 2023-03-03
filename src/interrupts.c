#include "interrupts.h"
#include "buttons.h"    // contains button pin definitions

#include "pico/stdlib.h"
#include <stdio.h>

void interrupts_init()
{
    buttons_init();
    gpio_set_irq_enabled_with_callback(buttons_s1_pin, GPIO_IRQ_EDGE_FALL, 1, isr_buttons);
    gpio_set_irq_enabled(buttons_s2_pin, GPIO_IRQ_EDGE_FALL, 1);
    gpio_set_irq_enabled(buttons_s3_pin, GPIO_IRQ_EDGE_FALL, 1);
}

void isr_buttons(uint gpio, uint32_t events)
{
    if (gpio == buttons_s1_pin) {
        printf("Interrupt occurred at pin %d with event %d\n", buttons_s1_pin, events);
    }
    else if (gpio == buttons_s2_pin) {
        printf("Interrupt occurred at pin %d with event %d\n", buttons_s2_pin, events);
    }
    else if (gpio == buttons_s3_pin) {
        printf("Interrupt occurred at pin %d with event %d\n", buttons_s3_pin, events);
    }
}
