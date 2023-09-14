#include "Nextion.h"
#include "fakes/streamDummy.hpp"
#include <gtest/gtest.h>

static StreamDummy stream;

static bool eventRegistered;
static void eventCallback(void *ptr) {
    eventRegistered = true;
}

static std::string lastLog;
static void logCallback(std::string log) {
    lastLog = log;
}

TEST(DisplayTestInteraction, PushEvents) {
    NxtIo::nexInit(stream, logCallback);

    Nxt::Page eut{0, "eut"};
    Nxt::Button btn{eut, 1, "btn"};
    btn.attachPush(eventCallback, &btn);

    NxtIo::SensingList list = {&btn};

    eventRegistered = false;
    stream.nextRead = {0x65,0x00,0x01,0x01,0xFF,0xFF,0xFF};
    NxtIo::nexLoop(list);

    EXPECT_EQ(true, eventRegistered);
    EXPECT_EQ("TouchEvent: 0 1 1", lastLog);
} 

TEST(DisplayTestInteractionIgnored, PushEvents) {
    NxtIo::nexInit(stream, logCallback);

    Nxt::Page eut{0, "eut"};
    Nxt::Button btn{eut, 1, "btn"};
    btn.attachPush(eventCallback, &btn);

    NxtIo::SensingList list = {&btn};
    
    eventRegistered = false;
    stream.nextRead = {0x65,0x00,0x02,0x01,0xFF,0xFF,0xFF};
    NxtIo::nexLoop(list);

    EXPECT_EQ(false, eventRegistered);
    EXPECT_EQ("TouchEvent ignored", lastLog);
} 