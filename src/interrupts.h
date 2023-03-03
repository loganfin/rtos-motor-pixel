#pragma once

#include "pico/stdlib.h"

void interrupts_init();
void isr_buttons(uint gpio, uint32_t events);
