#include "Nextion.hpp"
#include "fakes/streamDummy.hpp"
#include <gtest/gtest.h>

using namespace nxt;

static void logCallback(LogSeverity level, std::string log) {
    // _lastLog = log;
}

// TestFixture:
class NextionTest : public ::testing::Test {
  protected:
    void SetUp() override {
    }
    void TearDown() override {
    }

    NextionTest()
        : _stream(), _eut(_stream) {}

    bool eventRegistered;
    void eventCallback(void *ptr) {
        eventRegistered = true;
    }

    StreamDummy _stream;
    nxt::Port _eut;
    static std::string _lastLog;
};

std::string NextionTest::_lastLog;

TEST_F(NextionTest, EmptyBuffer) {
    _stream.setNextRead({});
    auto b = _eut.readCode(CodeBin::Return);
    EXPECT_EQ(0, b.size());
}

TEST_F(NextionTest, IncompleteBuffer0) {
    _stream.setNextRead({0x01});
    auto b = _eut.readCode(CodeBin::Return);
    EXPECT_EQ(0, b.size());
}

TEST_F(NextionTest, IncompleteBuffer1) {
    _stream.setNextRead({0x01, 0xFF});
    auto b = _eut.readCode(CodeBin::Return);
    EXPECT_EQ(0, b.size());
}

TEST_F(NextionTest, IncompleteBuffer2) {
    _stream.setNextRead({0x01, 0xFF, 0xFF});
    auto b = _eut.readCode(CodeBin::Return);
    EXPECT_EQ(0, b.size());
}

TEST_F(NextionTest, ReadInstructionSuccessful) {
    _stream.setNextRead(Return::InstructionSuccessful, {});
    auto b = _eut.readCode(CodeBin::Return);
    ASSERT_EQ(1, b.size());
    EXPECT_EQ(static_cast<int>(Return::InstructionSuccessful), b[0]);
}

TEST_F(NextionTest, ReadSerialBufferOverflow) {
    _stream.setNextRead(Return::SerialBufferOverflow, {});
    auto b = _eut.readCode(CodeBin::Return);
    ASSERT_EQ(1, b.size());
    EXPECT_EQ(static_cast<int>(Return::SerialBufferOverflow), b[0]);
}

TEST_F(NextionTest, ReadTouchEvent) {
    _stream.setNextRead(Return::TouchEvent, {0x01, 0x02, 0x03});
    auto b = _eut.readCode(CodeBin::Touch);
    ASSERT_EQ(4, b.size());
    EXPECT_EQ(static_cast<int>(Return::TouchEvent), b[0]);
}

TEST_F(NextionTest, ReadStringDataEnclosed) {
    _stream.setNextRead(Return::StringDataEnclosed, {'a', 'b', 'c'});
    auto b = _eut.readCode(CodeBin::String);
    ASSERT_EQ(4, b.size());
    EXPECT_EQ(static_cast<int>(Return::StringDataEnclosed), b[0]);
}

TEST_F(NextionTest, ReadNumericDataEnclosed) {
    _stream.setNextRead(Return::NumericDataEnclosed, {0x01, 0x02, 0x03, 0x04});
    auto b = _eut.readCode(CodeBin::Number);
    ASSERT_EQ(5, b.size());
    EXPECT_EQ(static_cast<int>(Return::NumericDataEnclosed), b[0]);
}

TEST_F(NextionTest, ReadingInTwoSteps) {
    _stream.setNextRead({0x01, 0xFF});
    auto b1 = _eut.readCode(CodeBin::Return);
    EXPECT_EQ(0, b1.size());
    _stream.setNextRead({0xFF, 0xFF});
    auto b2 = _eut.readCode(CodeBin::Return);
    ASSERT_EQ(1, b2.size());
    EXPECT_EQ(static_cast<int>(Return::InstructionSuccessful), b2[0]);
}
