#include "hdc.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

const uint hdc_address  = 0x40;
const uint hdc_temp_reg = 0x00;
const uint hdc_hmd_reg  = 0x01;
const uint hdc_config_reg  = 0x02;

void hdc_init()
{
    i2c_init(PICO_DEFAULT_I2C_INSTANCE, 100 * 1000);

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}
