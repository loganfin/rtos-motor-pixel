#include "buttons.h"
#include "hdc1080.h"
#include "interrupts.h"
#include "seg_display.h"
#include "stepper.h"
#include "tasks.h"

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

int main()
{
    stdio_init_all();

    interrupts_init();
    hdc1080_init();
    seg_display_init();
    stepper_init();

    xTaskCreate(vMotor, "MotorTask", 256, NULL, 5, NULL);
    vTaskStartScheduler();

    while (true) {
        printf("ERROR: Scheduler failed to start\n");
    }
}
