/**
 * @file ratioControl.hpp
 * @author Gustice
 * @brief Scaling utility for lights
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "ScaledValue.hpp"
#include "configModel.hpp"

/**
 * @brief Class to control a group of light by single intensity value
 */
class RatiometricLightControl {
  public:
    /// @brief Constructor
    /// @param config reference to configuration
    RatiometricLightControl(const StageConfig &config, const AmbientColorSet &active)
        : _config(config), _activeAmbient(active) {}

    /// @brief Update all wights by single intensity value
    /// @details scales all dmx channels by intensity values respect to configured weights
    /// @param intensities intensity value for illumination and ambient
    /// @return scaled values for DMX-output
    const StageValues &update(StageIntensity &intensities) {
        { // set values for illumination
            auto k = intensities.illumination;
            for (size_t i = 0; i < StageChannelsCount; i++) {
                _channels[i] = k * _config.weightsLights[i] / 0xFF;
            }
        }
        { // set values for background
            auto k = intensities.ambiance;
            auto col = _activeAmbient.backgroundColor;
            std::array<uint8_t, 3> cVal{col.red, col.green, col.blue};

            for (size_t c = 0; c < _config.channelsBackground.size(); c++) {
                auto i = _config.channelsBackground[c] - 1;
                _channels[i] = k * cVal[c] / 0xFF;
            }
        }
        { // set values for foreground
            auto k = intensities.ambiance;
            auto col = _activeAmbient.foregroundColor;
            std::array<uint8_t, 3> cVal{col.red, col.green, col.blue};

            for (size_t c = 0; c < _config.channelsForeground.size(); c++) {
                auto i = _config.channelsForeground[c] - 1;
                _channels[i] = k * cVal[c] / 0xFF;
            }
        }

        return _channels;
    }

  private:
    StageValues _channels;
    const StageConfig &_config;
    const AmbientColorSet _activeAmbient;
};