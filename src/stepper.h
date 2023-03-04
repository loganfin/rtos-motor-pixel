#pragma once

#include "pico/stdlib.h"

extern const uint stepper_coil_1_pin;
extern const uint stepper_coil_2_pin;
extern const uint stepper_coil_3_pin;
extern const uint stepper_coil_4_pin;
extern const uint stepper_rev_steps;

/* Stepper Motor Drivers */
void stepper_init();
void stepper_step(uint steps, bool clockwise);
void stepper_rev(bool clockwise);

/* FreeRTOS Tasks */
void vMotor();
