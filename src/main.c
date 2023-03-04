#include "buttons.h"
#include "hdc1080.h"
#include "seg_display.h"
#include "stepper.h"

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();   // enable serial monitoring

    buttons_init();     // initialize all pins tied to the buttons and set up the ISR
    hdc1080_init();     // initialize i2c bus
    seg_display_init(); // initialize all pins that are connected to the seven segement display
    stepper_init();     // initialize the pins connected to the four coils on the stepper motor

    xTaskCreate(vMotor, "MotorTask", 256, NULL, 5, NULL);
    vTaskStartScheduler();

    while (true) {
        printf("ERROR: Scheduler failed to start\n");
    }
}
