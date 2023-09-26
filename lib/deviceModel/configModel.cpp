
#include "configModel.hpp"
#include <sstream>

std::string StageConfig::getStageConfigStr() {
    std::stringstream output;
    output << "DMX-Layout:\n";
    output << "White-Weights\n";
    for (size_t i = 0; i < weightsLights.size(); i++) {
        auto wI = weightsLights[i];
        if (weightsLights[i] == 0) {
            continue;
        }
        output << "  " << i + 1 << ": " << (int)wI << "\n";
    }

    output << "Ambient-Foreground-RGB-Channels:\n";
    output << "  " << (int)channelsForeground[0] << " , " << (int)channelsForeground[1] << " , "
           << (int)channelsForeground[2] << "\n";
    output << "Ambient-Background-RGB-Channels:\n";
    output << "  " << (int)channelsBackground[0] << " , " << (int)channelsBackground[1] << " , "
           << (int)channelsBackground[2] << "\n";
    return output.str();
}

std::string DmxChannels::getValuesStr() {
    std::stringstream output;
    output << "DMX-Values:\n";

    for (size_t i = 0; i < values.size(); i++) {
        output << "  Ch " << i + 1 << " = " << (int)values[i] << "\n";
    }
    return output.str();
}

std::string DmxChannels::getValueStr(std::string ch) {
    auto i = std::stoi(ch);
    std::stringstream output;
    output << "  Ch " << i << " = " << (int)values[i-1] << "\n";
    return output.str();
}

Color Color::fromHue(uint8_t hue) {
        constexpr uint8_t MAX = UINT8_MAX;
        constexpr int DIV = MAX / 6;
        
        Color rgb;
        uint8_t region = hue / DIV;
        uint8_t remainder = hue % DIV;
        uint8_t q = (MAX * (MAX - ((MAX * remainder) >> 8))) >> 8;
        uint8_t t = (MAX * (MAX - ((MAX * (MAX - remainder)) >> 8))) >> 8;

        switch (region) {
        case 0:
            rgb.red = 255;
            rgb.green = t;
            rgb.blue = 0;
            break;
        case 1:
            rgb.red = q;
            rgb.green = 255;
            rgb.blue = 0;
            break;
        case 2:
            rgb.red = 0;
            rgb.green = 255;
            rgb.blue = t;
            break;
        case 3:
            rgb.red = 0;
            rgb.green = q;
            rgb.blue = 255;
            break;
        case 4:
            rgb.red = t;
            rgb.green = 0;
            rgb.blue = 255;
            break;
        default:
            rgb.red = 255;
            rgb.green = 0;
            rgb.blue = q;
            break;
        }

        return rgb;
}