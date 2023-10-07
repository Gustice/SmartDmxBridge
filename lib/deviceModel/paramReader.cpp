#include "paramReader.hpp"
#include "RapidJson.hpp"
#include <sstream>

static const char *LogTag = "FsReader";

const char *TAG = "JSON";

DeserializeException::DeserializeException(std::string message) {
    _message = "Deserialize Error: " + message;
}

DeserializeException::DeserializeException(std::vector<std::string> propertyChain, std::string message) {
    std::stringstream str;
    str << "Deserialize Error in: ";
    str << "'";
    for (auto &&p : propertyChain) {
        str << p << ".";
    }
    str << "'";
    str << ":  ";
    str << message;

    _message = str.str();
}

void readIlluminationWeights(rapidjson::Document &config, StageConfig &deviceSet) {
    if (!config.HasMember("illuminationWeights")) {
        throw DeserializeException("missing property 'illuminationWeights'");
    }

    const rapidjson::Value &wights = config["illuminationWeights"];
    if (!wights.IsArray() || wights.Size() != StageChannelsCount) {
        throw DeserializeException("'illuminationWeights' must contain an array of size=" + std::to_string(StageChannelsCount));
    }

    for (rapidjson::SizeType i = 0; i < wights.Size(); i++) {
        deviceSet.weightsLights[i] = wights[i].GetInt();
    }
}

void readColorChannelSet(const rapidjson::Value &channel, std::string propName, std::array<uint8_t, 3> &member) {

    if (!channel.IsArray() || channel.Size() != member.size()) {
        throw DeserializeException({propName}, "must contain an array of size=" + std::to_string(member.size()));
    }

    for (rapidjson::SizeType i = 0; i < member.size(); i++) {
        member[i] = channel[i].GetInt();
    }
}

void readColorChannels(rapidjson::Document &config, StageConfig &deviceSet) {
    if (!config.HasMember("ambientChannels")) {
        throw DeserializeException("missing property 'ambientChannels'");
    }
    const rapidjson::Value &ambient = config["ambientChannels"];

    if (!ambient.HasMember("foreground")) {
        throw DeserializeException("missing property 'ambientChannels.foreground'");
    }

    readColorChannelSet(ambient["foreground"], "ambientChannels.foreground", deviceSet.channelsForeground);

    if (!ambient.HasMember("background")) {
        throw DeserializeException("missing property 'ambientChannels.background'");
    }

    readColorChannelSet(ambient["background"], "ambientChannels.background", deviceSet.channelsBackground);
}

Color readPresetColor(const rapidjson::Value &channel, std::string propName) {

    if (!channel.IsArray() || channel.Size() != 3) {
        throw DeserializeException({propName}, "must contain array of size=" + std::to_string(3));
    }

    return Color{
        .red = static_cast<uint8_t>(channel[0].GetInt()),
        .green = static_cast<uint8_t>(channel[1].GetInt()),
        .blue = static_cast<uint8_t>(channel[2].GetInt())};
}

void readColorPreset(const rapidjson::Value &preset, AmbientColorSet &set) {

    if (!preset.HasMember("foreground")) {
        throw DeserializeException({"colorPresets", "foreground"}, "missing property ");
    }

    set.foregroundColor = readPresetColor(preset["foreground"], "ambientChannels.foreground");

    if (!preset.HasMember("background")) {
        throw DeserializeException({"colorPresets", "background"}, "missing property");
    }

    set.backgroundColor = readPresetColor(preset["background"], "ambientChannels.background");
}

void readColorPresets(rapidjson::Document &config, StageConfig &deviceSet) {
    if (!config.HasMember("colorPresets")) {
        throw DeserializeException("missing property 'colorPresets'");
    }
    const rapidjson::Value &presets = config["colorPresets"];

    if (!presets.IsArray() || presets.Size() != deviceSet.colorsPresets.size()) {
        throw DeserializeException("'colorPresets' must contain array of size=" + std::to_string(deviceSet.colorsPresets.size()));
    }

    for (rapidjson::SizeType i = 0; i < presets.Size(); i++) {
        readColorPreset(presets[i], deviceSet.colorsPresets[i]);
    }
}

StageConfig ParamReader::readDeviceConfig(const std::string serialized) {
    StageConfig deviceSet;
    rapidjson::Document config;
    if (config.Parse(serialized.c_str()).HasParseError()) {
        throw DeserializeException("Parsing error with error Code " + std::to_string(config.GetParseError()));
    }

    readIlluminationWeights(config, deviceSet);
    readColorChannels(config, deviceSet);
    readColorPresets(config, deviceSet);

    return deviceSet;
}
