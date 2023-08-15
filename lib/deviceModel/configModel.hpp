#pragma once

#include <array>
#include <cstdint>

#include <cstdint>
#include <string>

struct DmxChannels {
    std::array<uint8_t, 24> values;
    std::string getValuesStr();
    std::string getValueStr(std::string ch);
};

struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct AmbientColorSet {
    Color foregroundColor;
    Color backgroundColor;
};

struct StageConfig {
    std::array<uint8_t, 24> weightsLights;
    std::array<uint8_t, 3> channelsForeground;
    std::array<uint8_t, 3> channelsBackground;
    AmbientColorSet colors;

    std::string getStageConfigStr();
};

struct ColorPresets {
    AmbientColorSet preset1;
    AmbientColorSet preset2;
    AmbientColorSet preset3;
};

enum class DeviceMode {
    StandAlone,
    Remote,
    Manual,
    TestMode,
};
