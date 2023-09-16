#pragma once

#include <cstdint>
#include "driver/adc.h"
#include <array>

class Adc {
    public:
    Adc(adc_unit_t unit, adc_channel_t channel);

    static void calibrate(adc_unit_t unit);
    uint16_t readValue();

    private:
    const adc_unit_t _unit;
    const adc_channel_t _channel;
    static std::array<bool,adc_unit_t::ADC_UNIT_2 +1> _isInitialized;
};