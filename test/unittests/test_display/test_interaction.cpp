#include "Nextion.hpp"
#include "fakes/streamDummy.hpp"
#include <gtest/gtest.h>

using namespace nxt;
static const std::string EndSequence = "\xFF\xFF\xFF";
static std::string aESeq(std::string_view sv) {
    return std::string{sv.begin()} + EndSequence;
}

static bool eventRegistered;
static void eventCallback(void *ptr) {
    eventRegistered = true;
}

static std::string lastLog;
static void logCallback(nxt::LogSeverity level, std::string log) {
    lastLog = log;
}

// TestFixture:
class NextionInteractionTest : public ::testing::Test {
  protected:
    void SetUp() override {
    }
    void TearDown() override {
    }

    NextionInteractionTest()
        : _stream(), _eut(_stream, logCallback), _lWr(_stream.lastWrite) {}

    // bool eventRegistered;
    // void eventCallback(void *ptr) {
    //     eventRegistered = true;
    // }

    StreamDummy _stream;
    Nextion _eut;
    static std::string _lastLog;
    std::string &_lWr;
};
std::string NextionInteractionTest::_lastLog;


TEST_F(NextionInteractionTest, PushEvents) {
    nxt::Page page{_eut.createPage(0, "page")};
    page.show();
    auto eut = page.createComponent<nxt::Text>(1, "eut");
    nxt::Button btn = page.createComponent<nxt::Button>(1, "btn");
    btn.attachPush(eventCallback, &btn);

    Nextion::SensingList list = {&btn};

    eventRegistered = false;
    _stream.appendNextRead(nxt::Return::TouchEvent, {0x00, 0x01, 0x01});
    _eut.nexLoop(list);

    EXPECT_EQ(true, eventRegistered);
    EXPECT_EQ("TouchEvent: 0 1 1", lastLog);
}

TEST_F(NextionInteractionTest, IgnorePushEvents) {
    nxt::Page page{_eut.createPage(0, "page")};
    page.show();
    auto eut = page.createComponent<nxt::Text>(1, "eut");
    nxt::Button btn = page.createComponent<nxt::Button>(1, "btn");
    btn.attachPush(eventCallback, &btn);

    Nextion::SensingList list = {&btn};

    eventRegistered = false;
    _stream.appendNextRead(nxt::Return::TouchEvent, {0x00, 0x02, 0x01});
    _eut.nexLoop(list);

    EXPECT_EQ(false, eventRegistered);
    EXPECT_EQ("TouchEvent ignored", lastLog);
}

TEST_F(NextionInteractionTest, Timeout) {
    nxt::Page page{_eut.createPage(0, "page")};
    page.show();
    auto eut = page.createComponent<nxt::Text>(1, "eut");
    nxt::Button btn = page.createComponent<nxt::Button>(1, "btn");
    btn.attachPush(eventCallback, &btn);

    Nextion::SensingList list = {&btn};

    // One missing terminator
    _stream.appendNextRead({0x65, 0x00, 0x01, 0x01, 0xFF, 0xFF});

    _eut.nexLoop(list);
    EXPECT_EQ("recvRetCommandFinished timeout", lastLog);

    // No termination at all
    _stream.appendNextRead({0x65, 0x00, 0x01, 0x01});
    _eut.nexLoop(list);
    EXPECT_EQ("recvRetCommandFinished timeout", lastLog);
}


TEST_F(NextionInteractionTest, GLobalScopeTest) {
    nxt::Page pageVisible{_eut.createPage(0, "PageVis")};
    nxt::Page pageHidden{_eut.createPage(0, "PageHid")};
    _stream.appendNextRead(nxt::Return::InstructionSuccessful, {});
    pageVisible.show();
    
    nxt::Text txtOnVisiblePage = pageVisible.createComponent<nxt::Text>(1, "TxtVis");
    nxt::Text txtOnHiddenPage = pageHidden.createComponent<nxt::Text>(1, "TxtHid");
    
    txtOnVisiblePage.text.set("Test");
    EXPECT_EQ(_lWr, aESeq("TxtVis.txt=\"Test\""));
    txtOnVisiblePage.text.get();
    EXPECT_EQ(_lWr, aESeq("get TxtVis.txt"));

    txtOnHiddenPage.text.set("Test");
    EXPECT_EQ(_lWr, aESeq("PageHid.TxtHid.txt=\"Test\""));
    txtOnHiddenPage.text.get();
    EXPECT_EQ(_lWr, aESeq("get PageHid.TxtHid.txt"));

    pageHidden.show();
    // txtOnHiddenPage.text.set("Test");
    // EXPECT_EQ(_lWr, aESeq("TxtHid.txt=\"Test\""));
    // txtOnVisiblePage.text.set("Test");
    // EXPECT_EQ(_lWr, aESeq("PageVis.TxtVis.txt=\"Test\""));
}

TEST_F(NextionInteractionTest, InterruptDuringTransmission) {
    nxt::Page page{_eut.createPage(0, "page")};
    _stream.appendNextRead(nxt::Return::InstructionSuccessful, {});
    page.show();
    auto txt = page.createComponent<nxt::Text>(1, "eut");
    txt.attachPush(eventCallback, &txt);

    // Note: We assume that a touch event arrived during transmission of text-manipulation
    // so the touch event is first in queue when acknowledge is checked
    _stream.appendNextRead({static_cast<uint8_t>(nxt::Return::TouchEvent), 0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF,
    static_cast<uint8_t>(nxt::Return::InstructionSuccessful),  0xFF, 0xFF, 0xFF});

    txt.text.set("text");
    EXPECT_EQ(_lWr, aESeq("eut.txt=\"text\""));

    eventRegistered = false;
    Nextion::SensingList list = {&txt};
    _eut.nexLoop(list);

    EXPECT_EQ(true, eventRegistered);
    EXPECT_EQ("TouchEvent: 0 1 1", lastLog);
}

// static_cast<uint8_t>(nxt::Return::StringDataEnclosed), 0x61, 0x62, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF