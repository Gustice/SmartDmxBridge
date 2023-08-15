#pragma once

#include <cstdint>
#include "driver/adc.h"

class Adc {
    public:
    Adc(adc_unit_t unit, adc1_channel_t channel);

    static void calibrate(adc_unit_t unit);
    uint16_t readValue();

    private:
    const adc_unit_t _unit;
    const adc1_channel_t _channel;
};