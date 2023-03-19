#include "hdc.h"
#include "seg_display.h"
#include "stepper.h"

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "stdio.h"

const uint8_t hdc_address  = 0x40;
const uint8_t hdc_temp_reg = 0x00;
const uint8_t hdc_hmd_reg  = 0x01;
const uint8_t hdc_config_reg  = 0x02;

QueueHandle_t xQHDC;
QueueHandle_t xQHDCMotor;

void hdc_init()
{
    i2c_init(PICO_DEFAULT_I2C_INSTANCE, 100 * 1000);

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

void hdc_set_config(const uint16_t configuration)
{
    uint8_t packets[3] = {
        hdc_config_reg,
        (uint8_t)((configuration >> 8) & 0xff),
        (uint8_t)(configuration & 0xff),
    };

    i2c_write_blocking(PICO_DEFAULT_I2C_INSTANCE, hdc_address, packets, 3, false);
}

uint64_t ReadRegister(const uint8_t* regAddrVec, const uint8_t vecSize)
{
    uint8_t numBytes = vecSize * 2;
    uint8_t data[numBytes];
    uint64_t returnValue = 0;

    if (regAddrVec == NULL || vecSize < 1 || vecSize > 4) {
        return -1;
    }

    for (int i = 0; i < vecSize; i++) {
        i2c_write_blocking(PICO_DEFAULT_I2C_INSTANCE, hdc_address, &regAddrVec[i], 1, false);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        i2c_read_blocking(PICO_DEFAULT_I2C_INSTANCE, hdc_address, &data[i * 2], numBytes, false);
    }

    returnValue = data[0];
    for (int i = 0; i < numBytes-1; i++) {
        returnValue <<= 8;
        returnValue |= data[i+1];
    }

    return returnValue;
}

float hdc_temp_c()
{
    uint16_t rawTemp = ReadRegister(&hdc_temp_reg, 1);
    //uint16_t rawTemp = hdc_read_temp();
    float tempC = ((float)rawTemp / 65536) * 165 - 40;
    return tempC;
}

float hdc_temp_f()
{
    float tempC = hdc_temp_c();
    float tempF = (tempC * 9/5) + 32;
    return tempF;
}

float hdc_humidity()
{
    uint16_t rawHumid = ReadRegister(&hdc_hmd_reg, 1);
    float percentHumid = ((float)rawHumid / 65536) * 100;
    return percentHumid;
}

void vHDC()
{
    uint rx_data = 0;
    uint motor_data = 0;
    int tx_data = 0;
    uint current_data = 0;
    uint previous_data = 0;
    int current_temp = 0;
    int current_humidity = 0;
    display_packet tx_packet;

    while(true) {
        tx_packet.data = 0;
        tx_packet.duration = 500 / portTICK_PERIOD_MS;
        if (xQueueReceive(xQHDC, &rx_data, 0) == pdTRUE) {
            xQueueReset(xQControl);
            printf("queue cleared\n");
        }
        current_temp = hdc_temp_f();
        current_humidity = hdc_humidity();
        switch (rx_data) {
            case 'T':
                tx_packet.data = current_temp;
                xQueueSend(xQControl, &tx_packet, 0);
                vTaskDelay(10);
                break;
            case 'H':
                tx_packet.data = current_humidity;
                xQueueSend(xQControl, &tx_packet, 0);
                vTaskDelay(10);
                break;
            case 'S':
                break;
        }

        xQueueReceive(xQHDCMotor, &motor_data, 0);
        switch (motor_data) {
            case 't':
                current_data = current_temp;
                if (previous_data < current_data) {
                    tx_data = 1;
                }
                else if (previous_data == current_data) {
                    tx_data = 0;
                }
                else {
                    tx_data = -1;
                }
                xQueueSend(xQMotor, &tx_data, 0);
                previous_data = current_data;
                break;
            case 'h':
                current_data = current_humidity;
                if (previous_data < current_data) {
                    tx_data = 1;
                }
                else if (previous_data == current_data) {
                    tx_data = 0;
                }
                else {
                    tx_data = -1;
                }
                xQueueSend(xQMotor, &tx_data, 0);
                previous_data = current_data;
                break;
            case 's':
                break;
        }
        taskYIELD();
    }
}
