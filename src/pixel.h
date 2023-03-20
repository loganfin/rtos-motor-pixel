#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

extern QueueHandle_t xQPixel;

void vPixel(void* parameters);
