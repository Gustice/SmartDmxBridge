/**
 * @file adc.hpp
 * @author Gustice
 * @brief Abstraction of ADC Interface
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "driver/adc.h"
#include <array>
#include <cstdint>

/**
 * @brief ACD class
 */
class Adc {
  public:
    /// @brief Constructor
    /// @param unit ADC unit
    /// @param channel Channel of ADC unit
    Adc(adc_unit_t unit, adc_channel_t channel);

    /// @brief Read port
    /// @return read value
    uint16_t readValue();

  private:
    static void calibrate(adc_unit_t unit);
    const adc_unit_t _unit;
    const adc_channel_t _channel;
    static std::array<bool, adc_unit_t::ADC_UNIT_2 + 1> _isInitialized;
};