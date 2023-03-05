#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "pico/stdlib.h"

extern const uint seg_display_left;
extern const uint seg_display_right;

extern QueueHandle_t xQControl;

void seg_display_init();
void seg_display_digit(uint display, uint8_t digit);

/* FreeRTOS Tasks */
void vDisplayManager();
void vLeftDisplay();
void vRightDisplay();
