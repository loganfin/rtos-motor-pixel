#pragma once

#include "pico/stdlib.h"

extern const uint buttons_s1_pin;
extern const uint buttons_s2_pin;
extern const uint buttons_s3_pin;
extern const uint buttons_debounce_delay;

void buttons_init();
void isr_buttons(uint gpio, uint32_t events);

/* FreeRTOS Tasks */
void vButton1();
void vButton2();
void vButton3();

