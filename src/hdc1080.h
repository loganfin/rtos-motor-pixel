#pragma once

#include "pico/stdlib.h"

extern const uint hdc_address;
extern const uint hdc_temp_reg;
extern const uint hdc_hmd_reg;
extern const uint hdc_cfg_reg;

void hdc1080_init();
