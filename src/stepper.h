#pragma once

#include "pico/stdlib.h"

extern const uint stepper_coil_1_pin;
extern const uint stepper_coil_2_pin;
extern const uint stepper_coil_3_pin;
extern const uint stepper_coil_4_pin;

void stepper_init();
