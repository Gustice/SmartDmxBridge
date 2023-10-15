#include "Nextion.hpp"
#include "fakes/streamDummy.hpp"
#include <gtest/gtest.h>

using namespace nxt;

static void logCallback(LogSeverity level, std::string log) {
    // _lastLog = log;
}

// TestFixture:
class NextionSerialTest : public ::testing::Test {
  protected:
    void SetUp() override {
    }
    void TearDown() override {
    }

    NextionSerialTest()
        : _stream(), _eut(_stream) {}

    bool eventRegistered;
    void eventCallback(void *ptr) {
        eventRegistered = true;
    }

    StreamDummy _stream;
    nxt::Port _eut;
    static std::string _lastLog;
};

std::string NextionSerialTest::_lastLog;

TEST_F(NextionSerialTest, EmptyBuffer) {
    _stream.appendNextRead({});
    auto b = _eut.readCode(CodeBin::Return, 1, 0);
    EXPECT_EQ(0, b.size());
}

TEST_F(NextionSerialTest, IncompleteBuffer0) {
    _stream.appendNextRead({0x01});
    auto b = _eut.readCode(CodeBin::Return, 1, 0);
    EXPECT_EQ(0, b.size());
}

TEST_F(NextionSerialTest, IncompleteBuffer1) {
    _stream.appendNextRead({0x01, 0xFF});
    auto b = _eut.readCode(CodeBin::Return, 1, 0);
    EXPECT_EQ(0, b.size());
}

TEST_F(NextionSerialTest, IncompleteBuffer2) {
    _stream.appendNextRead({0x01, 0xFF, 0xFF});
    auto b = _eut.readCode(CodeBin::Return, 1, 0);
    EXPECT_EQ(0, b.size());
}

TEST_F(NextionSerialTest, ReadInstructionSuccessful) {
    _stream.appendNextRead(Return::InstructionSuccessful, {});
    auto b = _eut.readCode(CodeBin::Return, 1, 0);
    ASSERT_EQ(1, b.size());
    EXPECT_EQ(static_cast<int>(Return::InstructionSuccessful), b[0]);
}

TEST_F(NextionSerialTest, ReadSerialBufferOverflow) {
    _stream.appendNextRead(Return::SerialBufferOverflow, {});
    auto b = _eut.readCode(CodeBin::Return, 1, 0);
    ASSERT_EQ(1, b.size());
    EXPECT_EQ(static_cast<int>(Return::SerialBufferOverflow), b[0]);
}

TEST_F(NextionSerialTest, ReadTouchEvent) {
    _stream.appendNextRead(Return::TouchEvent, {0x01, 0x02, 0x03});
    auto b = _eut.readCode(CodeBin::Touch, 4, 0);
    ASSERT_EQ(4, b.size());
    EXPECT_EQ(static_cast<int>(Return::TouchEvent), b[0]);
}

TEST_F(NextionSerialTest, ReadStringDataEnclosed) {
    _stream.appendNextRead(Return::StringDataEnclosed, {'a', 'b', 'c'});
    auto b = _eut.readCode(CodeBin::String, -1, 0);
    ASSERT_EQ(4, b.size());
    EXPECT_EQ(static_cast<int>(Return::StringDataEnclosed), b[0]);
}

TEST_F(NextionSerialTest, ReadNumericDataEnclosed) {
    _stream.appendNextRead(Return::NumericDataEnclosed, {0x01, 0x02, 0x03, 0x04});
    auto b = _eut.readCode(CodeBin::Number, 5, 0);
    ASSERT_EQ(5, b.size());
    EXPECT_EQ(static_cast<int>(Return::NumericDataEnclosed), b[0]);
}

TEST_F(NextionSerialTest, ReadingInTwoSteps) {
    _stream.appendNextRead({0x01, 0xFF});
    _stream.appendNextRead({0xFF, 0xFF});
    auto b = _eut.readCode(CodeBin::Return, 1, 0);
    ASSERT_EQ(1, b.size());
    EXPECT_EQ(static_cast<int>(Return::InstructionSuccessful), b[0]);
}

TEST_F(NextionSerialTest, ReadingInTwoStepsAfterTimeOut) {
    _stream.appendNextRead({0x01, 0xFF});
    auto b1 = _eut.readCode(CodeBin::Return, 1, 0);
    EXPECT_EQ(0, b1.size());
    _stream.appendNextRead({0xFF, 0xFF});
    auto b2 = _eut.readCode(CodeBin::Return, 1, 0);
    ASSERT_EQ(1, b2.size());
    EXPECT_EQ(static_cast<int>(Return::InstructionSuccessful), b2[0]);
}
