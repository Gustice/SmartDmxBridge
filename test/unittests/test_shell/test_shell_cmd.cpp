#include <gtest/gtest.h>
#include <shell.hpp>

static std::string receivedCommand;
static Shell::Command::Status demoCmd_emtpy(Shell &shell, std::string input) {
    receivedCommand = input;
    return Shell::Command::Status::noError;
}
static Shell::Command::Status demoCmd_echoArgs(Shell &shell, std::string input) {
    receivedCommand = input;
    shell.printResponse(input);
    return Shell::Command::Status::noError;
}

static std::vector<Shell::Command> cmdTable{
    {"emptyCmd", demoCmd_emtpy},
    {"echoCmd", demoCmd_echoArgs},
};

// TestFixture:
class ShellCommandProcessingTest : public ::testing::Test {
  public:
    ShellCommandProcessingTest()
        : _config{"SUT", "V 1.0", "HelpPage"}, _shell(_buffer, _config, cmdTable),
          NewPromptLine(std::string{_shell.signs.newLine} + std::string{_shell.signs.prompt}) {}

  protected:
    void injectString(std::string input) {
        for (auto &&c : input) {
            _shell.consume(c);
        }
        _shell.consume('\n');
    }

    void SetUp() override {}

    void TearDown() override {}

    std::stringstream _buffer;
    Shell::Config _config;
    Shell _shell;
    std::string NewPromptLine;
};

TEST_F(ShellCommandProcessingTest, TriggerEmptyCommand) {
    std::string input = "emptyCmd arg1 arg2";
    injectString(input);
    EXPECT_EQ(receivedCommand, "arg1 arg2");
    EXPECT_EQ(_buffer.str(), input + _shell.signs.newLine + NewPromptLine);
}

TEST_F(ShellCommandProcessingTest, TriggerEchoCommand) {
    std::string input = "echoCmd arg1 arg2";
    injectString(input);
    EXPECT_EQ(receivedCommand, "arg1 arg2");
    EXPECT_EQ(_buffer.str(), input + _shell.signs.newLine + " <arg1 arg2" + NewPromptLine);
}
