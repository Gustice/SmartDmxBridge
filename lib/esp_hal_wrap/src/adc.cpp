#include "adc.hpp"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_adc_cal.h"

static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
// ADC_ATTEN_11db => FullScale = 3.3 V (supply on VDD_2)
static const adc_atten_t attenuation = ADC_ATTEN_DB_11;
static esp_adc_cal_characteristics_t adc_chars;
#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate

Adc::Adc(adc_unit_t unit, adc1_channel_t channel) : _unit(unit), _channel(channel) {
    if (_unit == adc_unit_t::ADC_UNIT_2) {
        ESP_LOGE("ADC", "ADC2 channel not implemented yet");
        return;
    } 
    calibrate(unit);
    gpio_num_t port;
    assert(adc1_pad_get_io_num(channel, &port) == ESP_OK);
    ESP_LOGI("ADC", "ADC_WIDTH_BIT_12 channel %d setup @GPIO %d ... ", channel, port);
    assert(adc1_config_width(width) == ESP_OK);
    esp_err_t rErr = adc1_config_channel_atten((adc1_channel_t)_channel, attenuation);

    if (rErr != ESP_OK)
        ESP_LOGW("ADC", "ADC1 channel setup issue %d: %s ", rErr, esp_err_to_name(rErr));
}

constexpr int NumberOfSamples = 4;
uint16_t Adc::readValue() {
    uint32_t adc_reading = 0;
    for (size_t i = 0; i < NumberOfSamples; i++)
    {
        adc_reading += adc1_get_raw((adc1_channel_t)_channel);
    }
    adc_reading /= NumberOfSamples;
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars);

    return (uint16_t)adc_reading;
}

void Adc::calibrate(adc_unit_t unit) {
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, attenuation, width, DEFAULT_VREF, &adc_chars);
}