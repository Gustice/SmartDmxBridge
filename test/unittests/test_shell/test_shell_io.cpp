#include <gtest/gtest.h>
#include <shell.hpp>

static std::vector<Shell::Command> emptyCmdTable;

TEST(ShellTestSuite, Construction) {
    std::stringstream buffer;
    Shell::Config cfg{
        .deviceName{"SUT"},
        .version{"V 1.0"},
        .help{"?? Help Page ??"},
    };
    Shell sut(buffer, cfg, emptyCmdTable);
}

// TestFixture:
class ShellIOTest : public ::testing::Test {
  public:
    ShellIOTest() : _config{"SUT", "V 1.0", "HelpPage"}, _shell(_buffer, _config, emptyCmdTable) {}

  protected:
    void SetUp() override {}

    void TearDown() override {}

    std::stringstream _buffer;
    Shell::Config _config;
    Shell _shell;
};

TEST_F(ShellIOTest, CheckWelcomeMessage) {
    _shell.printWelcome();

    EXPECT_EQ(_buffer.str(), "** SUT **\n* Get Help by typing ?-Symbol and return\n> ");
}

TEST_F(ShellIOTest, CheckVersionMessage) {
    _shell.printVersion();

    EXPECT_EQ(_buffer.str(), "V 1.0");
}

TEST_F(ShellIOTest, CheckHelpMessage) {
    _shell.printHelp();

    EXPECT_EQ(_buffer.str(), "HelpPage");
}

TEST_F(ShellIOTest, CheckResponseMessage) {
    _shell.printResponse("response");

    EXPECT_EQ(_buffer.str(), " <response");
}

TEST_F(ShellIOTest, CheckErrorMessage) {
    _shell.printError("some error");

    EXPECT_EQ(_buffer.str(), "!some error");
}

TEST_F(ShellIOTest, CheckEventMessage) {
    _shell.printEvent("some event");

    EXPECT_EQ(_buffer.str(), ":some event");
}

// TestFixture:
class ShellAsciiProcessingTest : public ::testing::Test {
  public:
    ShellAsciiProcessingTest()
        : _config{"SUT", "V 1.0", "HelpPage"}, _shell(_buffer, _config, emptyCmdTable) {}

  protected:
    void SetUp() override {}
    void TearDown() override {}

    std::stringstream _buffer;
    Shell::Config _config;
    Shell _shell;
};

TEST_F(ShellAsciiProcessingTest, IgnoreEndOfLine) {
    _shell.consume(-1);
    EXPECT_EQ(_buffer.str(), "");
}

TEST_F(ShellAsciiProcessingTest, IgnoreNotPrintable) {
    _shell.consume(0);
    EXPECT_EQ(_buffer.str(), "\n!unknown character -> discarding line ..\n> ");
}

TEST_F(ShellAsciiProcessingTest, PrintHelpPageOnRequest) {
    _shell.consume('?');
    _shell.consume('\n');
    EXPECT_EQ(_buffer.str(), "?\nHelpPage\n> ");
}

TEST_F(ShellAsciiProcessingTest, ArbitrarySequence) {
    _shell.consume('A');
    EXPECT_EQ(_buffer.str(), "A");
    _shell.consume('b');
    EXPECT_EQ(_buffer.str(), "Ab");
    _shell.consume('\b');
    EXPECT_EQ(_buffer.str(), "Ab\b");
    _shell.consume('A');
    EXPECT_EQ(_buffer.str(), "Ab\bA");
}
