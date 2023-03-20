#include "buttons.h"
#include "hdc.h"
#include "pixel.h"
#include "seg_display.h"
#include "stepper.h"

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include <stdio.h>

int main()
{
    stdio_init_all();   // enable serial monitoring
    buttons_init();     // initialize all pins tied to the buttons and set up the ISR
    hdc_init();         // initialize i2c bus
    seg_display_init(); // initialize all pins that are connected to the seven segement display
    stepper_init();     // initialize the pins connected to the four coils on the stepper motor

    xQMotorData = xQueueCreate(1, sizeof(uint));
    xQHDCMotor = xQueueCreate(1, sizeof(uint));
    xQHDC = xQueueCreate(1, sizeof(uint));
    xQControl   = xQueueCreate(11, sizeof(display_packet));
    xQLeftDisp  = xQueueCreate(1, sizeof(uint8_t));
    xQRightDisp = xQueueCreate(1, sizeof(uint8_t));
    xSemDisp    = xSemaphoreCreateBinary();

    xTaskCreate(vMotor, "MotorTask", 256, NULL, 3, NULL);
    xTaskCreate(vHDC, "HDCTask", 256, NULL, 3, NULL);
    xTaskCreate(vButton1, "Button1Task", 256, NULL, 4, NULL);
    xTaskCreate(vButton2, "Button2Task", 256, NULL, 4, NULL);
    xTaskCreate(vButton3, "Button3Task", 256, NULL, 4, NULL);
    xTaskCreate(vDisplayManager, "DisplayManagerTask", 256, NULL, 3, NULL);
    xTaskCreate(vLeftDisplay, "LeftDisplayTask", 256, NULL, 3, NULL);
    xTaskCreate(vRightDisplay, "RightDisplayTask", 256, NULL, 3, NULL);
    xTaskCreate(vPixel, "PixelTask", 256, NULL, 3, NULL);
    vTaskStartScheduler();

    while (true) {
        printf("ERROR: Scheduler failed to start\n");
    }
}

