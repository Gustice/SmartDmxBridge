/**
 * @file shell.hpp
 * @author Gustice
 * @brief Shell-Source for interactive control of the device. See docs for further details.
 */

#pragma once

#include <sstream>
#include <string_view>
#include <vector>

class Shell {
  public:
    struct Config {
        std::string_view deviceName;
        std::string_view version;
        std::string_view help;
    };

    struct Signs {
        std::string_view prompt;
        std::string_view response;
        std::string_view event;
        std::string_view error;
        char newLine;
        char backspace;
        std::string_view getHelp;
    };

    struct Command {
        enum class [[nodiscard]] Status{noError, Error};
        using Callback = Status (*)(Shell &shell, std::string input);

        std::string_view code;
        Callback call;
    };

    Shell(std::ostream &oStream, const Config &config, const std::vector<Command> &cmdTable);

    void printWelcome();
    void printVersion();
    void printHelp();
    void printEvent(const std::string &msg);
    void printError(const std::string &msg);
    void printResponse(const std::string &msg);
    void consume(int input);
    void consume(std::string input);

    const Signs &signs;

  private:
    std::ostream &_output;
    const Config &_config;
    std::stringstream _input;
    void setupNewLine();
    void processString(std::string input);
    std::string _command;
    const std::vector<Command> &_commandTable;
};
