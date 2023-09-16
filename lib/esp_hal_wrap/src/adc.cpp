#include "adc.hpp"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_system.h"

static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
// ADC_ATTEN_11db => FullScale = 3.3 V (supply on VDD_2)
static const adc_atten_t attenuation = ADC_ATTEN_DB_11;
static esp_adc_cal_characteristics_t adc_chars[adc_unit_t::ADC_UNIT_2 + 1];
#define DEFAULT_VREF 1100 // Use adc2_vref_to_gpio() to obtain a better estimate

std::array<bool, adc_unit_t::ADC_UNIT_2 + 1> Adc::_isInitialized;

Adc::Adc(adc_unit_t unit, adc_channel_t channel) : _unit(unit), _channel(channel) {

    if ((unit != ADC_UNIT_1) && (unit != ADC_UNIT_2)) {
        ESP_LOGE("ADC", "ADC-unit %d not supported", _unit);
        return;
    }

    if (!_isInitialized[_unit]) {
        calibrate(unit);
        adc1_config_width(width);
        _isInitialized[_unit] = true;
    }

    switch (_unit) {
    case adc_unit_t::ADC_UNIT_1: {

        if (channel >= adc1_channel_t::ADC1_CHANNEL_MAX) {
            ESP_LOGE("ADC", "ADC-channel %d not valid", channel);
        }
        auto ch = static_cast<adc1_channel_t>(channel);

        adc1_config_channel_atten(ch, attenuation);

        gpio_num_t port;
        assert(adc1_pad_get_io_num(ch, &port) == ESP_OK);
        ESP_LOGI("ADC", "ADC_WIDTH_BIT_12 channel %d setup @GPIO %d ... ", channel, port);
        assert(adc1_config_width(width) == ESP_OK);
        esp_err_t rErr = adc1_config_channel_atten(ch, attenuation);

        if (rErr != ESP_OK)
            ESP_LOGW("ADC", "ADC1 channel setup issue %d: %s ", rErr, esp_err_to_name(rErr));

    } break;

    case adc_unit_t::ADC_UNIT_2: {

        if (channel >= adc2_channel_t::ADC2_CHANNEL_MAX) {
            ESP_LOGE("ADC", "ADC-channel %d not valid", channel);
        }
        auto ch = static_cast<adc2_channel_t>(channel);

        gpio_num_t port;
        assert(adc2_pad_get_io_num(ch, &port) == ESP_OK);
        ESP_LOGI("ADC", "channel %d setup @GPIO %d ... ", channel, port);
        assert(adc1_config_width(width) == ESP_OK);
        esp_err_t rErr = adc2_config_channel_atten(ch, attenuation);

        if (rErr != ESP_OK)
            ESP_LOGW("ADC", "ADC1 channel setup issue %d: %s ", rErr, esp_err_to_name(rErr));

    } break;

    default:
        ESP_LOGE("ADC", "ADC-unit %d not implemented", _unit);
        break;
    }
}

constexpr int NumberOfSamples = 4;
uint16_t Adc::readValue() {
    uint32_t adc_reading = 0;

    switch (_unit) {

    case adc_unit_t::ADC_UNIT_1: {
        for (size_t i = 0; i < NumberOfSamples; i++) {
            adc_reading += adc1_get_raw((adc1_channel_t)_channel);
        }
    } break;

    case adc_unit_t::ADC_UNIT_2: {
        for (size_t i = 0; i < NumberOfSamples; i++) {
            int raw;
            adc2_get_raw((adc2_channel_t)_channel, width, &raw);
            adc_reading += raw;
        }
    } break;

    default:
        break;
    }

    adc_reading = adc_reading / NumberOfSamples;
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars[_unit]);
    return (uint16_t)adc_reading;
}

void Adc::calibrate(adc_unit_t unit) {
    esp_adc_cal_value_t val_type =
        esp_adc_cal_characterize(unit, attenuation, width, DEFAULT_VREF, &adc_chars[unit]);
}
