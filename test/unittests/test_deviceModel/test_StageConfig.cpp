#include "configModel.hpp"
#include <gtest/gtest.h>

TEST(DeviceModelTest, TestStageOutput) {
    StageConfig dut{
        .weightsLights{
            11, 12, 13, 0, 0, 0,
            17, 18, 19, 0, 0, 0,
            23, 24, 25, 0, 0, 0,
            29, 30, 31, 0, 0, 0},
        .channelsForeground{4, 5, 6},
        .channelsBackground{10, 11, 12},
        .colorsPresets{
            AmbientColorSet{
                .foregroundColor{50, 60, 70},
                .backgroundColor{80, 90, 100},
            },
            {
                .foregroundColor{51, 61, 71},
                .backgroundColor{81, 91, 101},
            },
            {
                .foregroundColor{52, 62, 72},
                .backgroundColor{82, 92, 102},
            }}};

    std::string expect{
        "DMX-Layout:\n"
        "White-Weights\n"
        "  1: 11\n"
        "  2: 12\n"
        "  3: 13\n"
        "  7: 17\n"
        "  8: 18\n"
        "  9: 19\n"
        "  13: 23\n"
        "  14: 24\n"
        "  15: 25\n"
        "  19: 29\n"
        "  20: 30\n"
        "  21: 31\n"
        "Ambient-Foreground-RGB-Channels:\n"
        "  4 , 5 , 6\n"
        "Ambient-Background-RGB-Channels:\n"
        "  10 , 11 , 12\n"

    };

    EXPECT_EQ(expect, dut.getStageConfigStr());
}

TEST(DeviceModelTest, TestDmxChannels) {
    DmxChannels dut{
        .values{
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24}};

    std::string expect{
        "DMX-Values:\n"
        "  Ch 1 = 1\n"
        "  Ch 2 = 2\n"
        "  Ch 3 = 3\n"
        "  Ch 4 = 4\n"
        "  Ch 5 = 5\n"
        "  Ch 6 = 6\n"
        "  Ch 7 = 7\n"
        "  Ch 8 = 8\n"
        "  Ch 9 = 9\n"
        "  Ch 10 = 10\n"
        "  Ch 11 = 11\n"
        "  Ch 12 = 12\n"
        "  Ch 13 = 13\n"
        "  Ch 14 = 14\n"
        "  Ch 15 = 15\n"
        "  Ch 16 = 16\n"
        "  Ch 17 = 17\n"
        "  Ch 18 = 18\n"
        "  Ch 19 = 19\n"
        "  Ch 20 = 20\n"
        "  Ch 21 = 21\n"
        "  Ch 22 = 22\n"
        "  Ch 23 = 23\n"
        "  Ch 24 = 24\n"};

    EXPECT_EQ(expect, dut.getValuesStr());

    EXPECT_EQ("  Ch 1 = 1\n", dut.getValueStr("1"));
    EXPECT_EQ("  Ch 12 = 12\n", dut.getValueStr("12"));
    EXPECT_EQ("  Ch 24 = 24\n", dut.getValueStr("24"));
}