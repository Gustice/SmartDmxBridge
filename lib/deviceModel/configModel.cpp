
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

