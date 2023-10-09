/**
 * @file configModel.hpp
 * @author Gustice
 * @brief Definitions and models
 * @date 2023-10-04
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>

/// @brief Number of DMX-Channels
constexpr int StageChannelsCount = 24;

/// @brief Type definition for Array of DMX-Channels
using StageValues = std::array<uint8_t, StageChannelsCount>;

/**
 * @brief DMX-Channel complex
 */
struct DmxChannels {
    /// @brief Values
    std::vector<uint8_t> values;

    /// @brief Create descriptive string from values for terminal output
    /// @return generated string
    std::string getValuesStr();

    /// @brief Create descriptive string for one value for terminal output
    /// @return generated string
    std::string getValueStr(std::string ch);
};

/**
 * @brief Generic color value
 */
struct Color {
    /// @brief Red
    uint8_t red;

    /// @brief Green
    uint8_t green;

    /// @brief Blue
    uint8_t blue;

    /// @brief Create color value from Hue  (only hue value is respected)
    /// @param hue hue
    /// @return Color structure
    static Color fromHue(uint8_t hue);
};

/**
 * @brief Color set for ambient theme. 
 * @details Ambiente illumination is to create/support a mood consisting of hopefully harmonious colors
 *   Illumination in turn is just to illuminate the stage
 */
struct AmbientColorSet {
    /// @brief Color for Foreground
    Color foregroundColor;

    /// @brief Color for Background
    Color backgroundColor;
};

/// @brief Shortened definition for set of colors as preset
using ColorPresets = std::array<AmbientColorSet, 3>;

/**
 * @brief Stage configuration for lights. 
 * @details Defines the weights for the channels that are scaled with the intensity values 
 *   and written to DMX-output
 */
struct StageConfig {
    /// @brief Weights for illumination channels
    std::array<uint8_t, StageChannelsCount> weightsLights{};

    /// @brief Channel indexes for foreground color
    std::array<uint8_t, 3> channelsForeground{};

    /// @brief Channel indexes for background color
    std::array<uint8_t, 3> channelsBackground{};

    /// @brief Color presets
    ColorPresets colorsPresets{};

    /// @brief Create descriptive string for terminal output
    /// @return generated string
    std::string getStageConfigStr();
};

/**
 * @brief Intensities that are applied on weights defined in stage configuration
 */
struct StageIntensity {
    /// @brief Intensity for illumination
    uint8_t illumination = 0;

    /// @brief Intensity for ambient color set
    uint8_t ambiente = 0;
};

/**
 * @brief Device stat/ mode of operation
 */
class DeviceState {
  public:
    /**
   * @brief Enumeration for Mode
   */
    enum class Mode {
        /// @brief Standalone mode:
        /// Configuration weights are scaled with potentiometer values send via DMX-output
        StandAlone,

        /// @brief Remote mode:
        /// Applies values that are received via ArtNet, potentiometer values are muted
        Remote,

        /// @brief Manual mode:
        /// Debugging and testing via Telnet shell
        Manual,

        /// @brief Test ring mode:
        /// Sends current channel values cyclically and compares them with received values
        /// Throws errors via Syslog if values don't match
        TestRing,

        /// @brief Passive sensing:
        /// Receiving values and comparing them with last receive value
        /// Throws diff block via Syslog if values don't match
        Sensing,
    };

    /// @brief Constructor
    /// @param init Initial state
    DeviceState(Mode init)
        : _default(init), _current(init), _last(init) {}

    /// @brief Compare state with argument
    /// @param mode expected string
    /// @return true if mode matches actual state
    bool stateIs(Mode mode) {
        return _current == mode;
    }

    /// @brief Get current state
    /// @return current state
    Mode getState() {
        return _current;
    }

    /// @brief Set state
    /// @param mode new state
    void setNewState(Mode mode) {
        _last = _current;
        _current = mode;
    }

    /// @brief Fallback to last state
    void fallbackToLast() {
        _current = _last;
    }

  private:
    const Mode _default;
    Mode _current;
    Mode _last;
};

// clang-format off
constexpr StageConfig DefaultStageConfig{ 
    .weightsLights{
        /*Illumination*/  255, 255, 255, 255, //  1- 4
        /*Spare*/           0,   0,   0,   0, //  5- 8
        /*Spare*/           0,   0,   0,   0, //  9-12
        /*Spare*/           0,   0,   0,   0, // 13-16
        /*Foreground*/      0,   0,   0,   0, // 17-20
        /*Background*/      0,   0,   0,   0, // 21-24
    },
    .channelsForeground{17, 18, 19},
    .channelsBackground{21, 22, 23},
    .colorsPresets{ 
        AmbientColorSet{ 
            .foregroundColor{255, 0, 0},
            .backgroundColor{0, 127, 127},
        },
        AmbientColorSet{
            .foregroundColor{0, 255, 0},
            .backgroundColor{127, 0, 127},
        },
        AmbientColorSet{
            .foregroundColor{0, 0, 255},
            .backgroundColor{127, 127, 0},
        }
        }
    };

constexpr std::string_view DeviceConfigFilename{"Configuration.json"};