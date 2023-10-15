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

static std::queue<std::string> logBuffer;
static void logCallback(nxt::LogSeverity level, std::string log) {
    logBuffer.push(log);
}

// TestFixture:
class NextionFullDemo : public ::testing::Test {
  protected:
    void SetUp() override {
    }
    void TearDown() override {
    }

    NextionFullDemo()
        : _stream(), _lWr(_stream.lastWrite) {
        _stream.appendNextRead(nxt::Return::InstructionSuccessful, {}); // for initial bkcmd-command
        _stream.appendNextRead(nxt::Return::InstructionSuccessful, {}); // for initial page-command
    }

    // bool eventRegistered;
    // void eventCallback(void *ptr) {
    //     eventRegistered = true;
    // }

    StreamDummy _stream;
    static std::string _lastLog;
    std::string &_lWr;
};
std::string NextionFullDemo::_lastLog;

TEST_F(NextionFullDemo, PushEvents) {

    Nextion eut(_stream, logCallback);

    ASSERT_EQ(2, logBuffer.size());
    EXPECT_EQ("recvRetCommandFinished ok", logBuffer.front());
    logBuffer.pop();
    EXPECT_EQ("recvRetCommandFinished ok", logBuffer.front());
    logBuffer.pop();

    nxt::Page page1{eut.createPage(0, "page1")};
    auto bP1Btn = page1.createComponent<nxt::Button>(1, "bP1Btn");
    auto tP1Txt = page1.createComponent<nxt::Text>(2, "tP1Txt");

    nxt::Page page2{eut.createPage(0, "page2")};
    auto b21Btn = page1.createComponent<nxt::Button>(1, "bP1Btn");
    auto t21Txt = page1.createComponent<nxt::Text>(2, "tP1Txt");

    EXPECT_EQ(0, logBuffer.size());

    _stream.appendNextRead(nxt::Return::InstructionSuccessful, {}); // for initial bkcmd-command
    page1.show();
    EXPECT_EQ("recvRetCommandFinished ok", logBuffer.front());
    logBuffer.pop();

    // auto eut = page.createComponent<nxt::Text>(1, "eut");
    // nxt::Button btn = page.createComponent<nxt::Button>(1, "btn");
    // btn.attachPush(eventCallback, &btn);

    // Nextion::SensingList list = {&btn};

    // eventRegistered = false;
    // _stream.appendNextRead(nxt::Return::TouchEvent, {0x00, 0x01, 0x01});
    // eut.nexLoop(list);

    // EXPECT_EQ(true, eventRegistered);
    // EXPECT_EQ("TouchEvent: 0 1 1", lastLog);
}
