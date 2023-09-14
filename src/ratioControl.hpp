#pragma once

#include "ScaledValue.hpp"
#include "configModel.hpp"

class RatiometricLightControl {
  public:
    RatiometricLightControl(const StageConfig &config) : _config(config) {}

    const StageValues& update(StageIntensity &intensities) {
        {
            auto k = intensities.illumination;
            for (size_t i = 0; i < StageChannelsCount; i++) {
                _channels[i] = k * _config.weightsLights[i] / 0xFF;
            }
        }

        {
            auto k = intensities.ambiente;
            auto col = _config.colors.backgroundColor;
            std::array<uint8_t, 3> cVal{col.red, col.green, col.blue};

            for (size_t c = 0; c < _config.channelsBackground.size(); c++) {
                auto i = _config.channelsBackground[c] -1;
                _channels[i] = k * cVal[c] / 0xFF;
            }
        }

        {
            auto k = intensities.ambiente;
            auto col = _config.colors.foregroundColor;
            std::array<uint8_t, 3> cVal{col.red, col.green, col.blue};

            for (size_t c = 0; c < _config.channelsForeground.size(); c++) {
                auto i = _config.channelsForeground[c] -1;
                _channels[i] = k * cVal[c] / 0xFF;
            }
        }

        return _channels;
    }

  private:
    StageValues _channels;
    const StageConfig &_config;
};