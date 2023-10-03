#pragma once

#include <array>
#include <vector>
#include <cstdint>

#include <cstdint>
#include <string>

constexpr int StageChannelsCount = 24;
using StageValues = std::array<uint8_t, StageChannelsCount>;

struct DmxChannels {
    std::vector<uint8_t> values;
    std::string getValuesStr();
    std::string getValueStr(std::string ch);
};

struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    static Color fromHue(uint8_t hue); 
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

struct StageIntensity {
    uint8_t illumination = 0;
    uint8_t ambiente = 0;
};

struct ColorPresets {
    AmbientColorSet preset1;
    AmbientColorSet preset2;
    AmbientColorSet preset3;
};

class DeviceState {
  public:
    enum class Mode {
        StandAlone,
        Remote,
        Manual,
        TestRing,
        Sensing,
    };

    DeviceState(Mode init) : _default(init), _current(init), _last(init) { }

    bool stateIs(Mode mode) {
        return _current == mode;
    }

    Mode getState() {
        return _current;
    }

    void setNewState(Mode mode) {
        _last = _current;
        _current = mode;
    }

    void fallbackToLast() {
        _current = _last;
    }

  private:
    const Mode _default;
    Mode _current;
    Mode _last;
};
