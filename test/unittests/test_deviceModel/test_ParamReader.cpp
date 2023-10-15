#include "paramReader.hpp"
#include <gtest/gtest.h>

std::string fullDescriptor{
    "{ \"dmxOutputs\": 24, \
   \"illuminationWeights\": [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 ], \
   \"ambientChannels\": { \
       \"foreground\": [ 3, 2, 1 ], \
       \"background\": [ 11, 12, 13 ] }, \
   \"constants\": [ 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34 ], \
    \"colorPresets\": [ \
        { \"foreground\": [ 31, 32, 33 ], \
          \"background\": [ 41, 42, 43 ] }, \
        { \"foreground\": [ 51, 52, 53 ], \
          \"background\": [ 61, 62, 63 ] }, \
        { \"foreground\": [ 71, 72, 73 ], \
          \"background\": [ 81, 82, 83 ] } \
    ] }"};

TEST(InterpreterTest, ReadFullJSON) {
    try {
        StageConfig device = ParamReader::readDeviceConfig(fullDescriptor);

        for (size_t i = 0; i < device.weightsLights.size(); i++) {
            EXPECT_EQ(i + 1, device.weightsLights[i]) << " item " << i;
        }
        for (size_t i = 0; i < device.constants .size(); i++) {
            EXPECT_EQ(i + 11, device.constants[i]) << " item " << i;
        }

        EXPECT_EQ(3, device.channelsForeground[0]);
        EXPECT_EQ(2, device.channelsForeground[1]);
        EXPECT_EQ(1, device.channelsForeground[2]);

        EXPECT_EQ(11, device.channelsBackground[0]);
        EXPECT_EQ(12, device.channelsBackground[1]);
        EXPECT_EQ(13, device.channelsBackground[2]);

        EXPECT_EQ(31, device.colorsPresets[0].foregroundColor.red);
        EXPECT_EQ(32, device.colorsPresets[0].foregroundColor.green);
        EXPECT_EQ(33, device.colorsPresets[0].foregroundColor.blue);
        EXPECT_EQ(41, device.colorsPresets[0].backgroundColor.red);
        EXPECT_EQ(42, device.colorsPresets[0].backgroundColor.green);
        EXPECT_EQ(43, device.colorsPresets[0].backgroundColor.blue);

        EXPECT_EQ(51, device.colorsPresets[1].foregroundColor.red);
        EXPECT_EQ(52, device.colorsPresets[1].foregroundColor.green);
        EXPECT_EQ(53, device.colorsPresets[1].foregroundColor.blue);
        EXPECT_EQ(61, device.colorsPresets[1].backgroundColor.red);
        EXPECT_EQ(62, device.colorsPresets[1].backgroundColor.green);
        EXPECT_EQ(63, device.colorsPresets[1].backgroundColor.blue);

        EXPECT_EQ(71, device.colorsPresets[2].foregroundColor.red);
        EXPECT_EQ(72, device.colorsPresets[2].foregroundColor.green);
        EXPECT_EQ(73, device.colorsPresets[2].foregroundColor.blue);
        EXPECT_EQ(81, device.colorsPresets[2].backgroundColor.red);
        EXPECT_EQ(82, device.colorsPresets[2].backgroundColor.green);
        EXPECT_EQ(83, device.colorsPresets[2].backgroundColor.blue);

    } catch (const std::exception &e) {
        FAIL() << "Exception occurred " << e.what();
    }
}

bool stringContains(std::string haystack, std::string needle) {
    if (haystack.find(needle) != std::string::npos) {
        return true;
    }
    return false;
}

