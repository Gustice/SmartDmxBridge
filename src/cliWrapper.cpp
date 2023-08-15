#include "cliWrapper.hpp"

const std::string_view WelcomeString {
    "** Smart-DMX-Bridge **\n"
    "  send 'help' for help-page\n"
};

StageConfig * Ui::_stage;
DmxChannels * Ui::_channels;
OtaHandler * Ui::_ota;
