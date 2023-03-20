#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "pico/stdlib.h"

extern const uint seg_display_left;
extern const uint seg_display_right;

extern QueueHandle_t xQControl;
extern QueueHandle_t xQLeftDisp;
extern QueueHandle_t xQRightDisp;
//extern SemaphoreHandle_t xSemDisp;
extern SemaphoreHandle_t xSemDisp;

typedef struct display_packet {
    int data;
    uint duration;
} display_packet;

void seg_display_init();
void seg_display_digit(uint display, uint8_t digit);
void seg_display_off();

/* FreeRTOS Tasks */
void vDisplayManager(void* parameters);
void vLeftDisplay(void* parameters);
void vRightDisplay(void* parameters);
