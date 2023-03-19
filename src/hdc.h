#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "pico/stdlib.h"

extern QueueHandle_t xQHDC;
extern QueueHandle_t xQHDCMotor;

void hdc_init();

void vHDC();
