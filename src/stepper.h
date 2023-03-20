#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "pico/stdlib.h"

extern const uint stepper_clockwise;
extern const uint stepper_rev_steps;

extern QueueHandle_t xQMotor;
extern QueueHandle_t xQMotorData;

/* Stepper Motor Drivers */
void stepper_init();
void stepper_step(uint steps, uint direction);
void stepper_rev(uint clockwise);

/* FreeRTOS Tasks */
void vMotor(void* parameters);
