#include "cliWrapper.hpp"

const std::string_view WelcomeString {
    "** Smart-DMX-Bridge **\n"
    "  send 'help' for help-page\n"
};

Ui::Config * Ui::_config;
Cli * Ui::_rShell = nullptr;
Ui::abortCallback Ui::_abortCbk = nullptr;