#include "tasks.h"
#include "stepper.h"


#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"

void vMotor()
{
    while(true) {
        stepper_rev(true);
    }
}
