#include "shell.hpp"
#include <cstring>

constexpr Shell::Signs shellSigns{.prompt{"> "},
    .response{" <"},
    .event{":"},
    .error{"!"},
    .newLine{'\n'},
    .backspace{'\b'},
                                  .getHelp{"?"}};

void Shell::printWelcome() {
    _output << "** " << _config.deviceName << " **\n";
    _output << "* Get Help by typing ?-Symbol and return\n";
    setupNewLine();
}

void Shell::printVersion() {
    _output << _config.version;
}

void Shell::printHelp() {
    _output << _config.help;
}

void Shell::printEvent(const std::string &msg) {
    _output << shellSigns.event << msg;
}

void Shell::printError(const std::string &msg) {
    _output << shellSigns.error << msg;
}

void Shell::printResponse(const std::string &msg) {
    _output << shellSigns.response << msg;
}

void Shell::setupNewLine() {
    _output << shellSigns.prompt;
}

void Shell::consume(int input) {
    if (input < 0) // eof input ...
        return;

    char c = (char)input;
    // ignore DOS-return (accept only '\n')
    if (c == '\r')
        return;

    if (c == shellSigns.backspace) {
        _output << c;
        _command.pop_back();
        return;
    }

    if (c == shellSigns.newLine) {
        _output << c;
        processString(_command);
        setupNewLine();
        return;
    }

    if (!isprint(c) || c > '~') {
        _output << shellSigns.newLine;
        printError("unknown character -> discarding line ..\n");
        setupNewLine();
        return;
    }

    _output << c;
    _command.push_back(c);
}

void Shell::consume(std::string input) {}

void Shell::processString(std::string input) {
    if (input.empty()) {
        return;
    }

    if (input == "?") {
        printHelp();
        _output << shellSigns.newLine;
        return;
    }

    printError("Unknown command. See help with '?' ...");
}
