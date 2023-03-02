#include "buttons.h"
#include "hdc1080.h"
#include "interrupts.h"
#include "seg_display.h"
#include "stepper.h"

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

int main()
{
    stdio_init_all();

    buttons_init();
    hdc1080_init();
    seg_display_init();
    stepper_init();
}
