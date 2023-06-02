/**
 * @file shell.hpp
 * @author Gustice
 * @brief Shell-Source for interactive control of the device. See docs for further details.
 */

#pragma once

#include <sstream>
#include <string_view>

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

    Shell(std::ostream &oStream, Config config) : _output(oStream), _config(config) {}

    void printWelcome();
    void printVersion();
    void printHelp();
    void printEvent(const std::string &msg);
    void printError(const std::string &msg);
    void printResponse(const std::string &msg);
    void consume(int input);
    void consume(std::string input);

  private:
    Config _config;
    std::stringstream _input;
    std::ostream &_output;
    void setupNewLine();
    void processString(std::string input);
    std::string _command;
};
