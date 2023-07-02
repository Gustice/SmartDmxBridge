#pragma once

#include<cstdint>
#include<array>

struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct AmbientColorSet
{
    Color foregroundColor;
    Color backgroundColor;
};

struct StageConfig {
    std::array<uint8_t, 24> weightsLights;
    std::array<uint8_t, 3> channelsForeground;
    std::array<uint8_t, 3> channelsBackground;
    AmbientColorSet colors;
};

struct ColorPresets {
    AmbientColorSet preset1;
    AmbientColorSet preset2;
    AmbientColorSet preset3;
};