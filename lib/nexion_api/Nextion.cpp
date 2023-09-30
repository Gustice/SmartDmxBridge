#include "Nextion.hpp"
// #include "Page.hpp"
#include <numeric>
#include <string_view>

using namespace nxt;

void Nextion::nexLoop(const SensingList &nex_listen_list) {
    constexpr unsigned ES = 4;
    auto input = _port.readCode(CodeBin::Touch, ES, 0);

    if (input.size() != ES) {
        return;
    }

    if (static_cast<Return>(input[0]) != Return::TouchEvent) {
        Nextion::sendLog(LogSeverity::Warning, "nexLoop err");
    }

    uint8_t page = input[1];
    uint8_t component = input[2];
    int32_t event = (int32_t)input[3];

    Nextion::sendLog(LogSeverity::Debug, std::string{"TouchEvent: "} +
                                             std::to_string(static_cast<int>(input[1])) + " " +
                                             std::to_string(static_cast<int>(input[2])) + " " +
                                             std::to_string(static_cast<int>(input[3])));

    if (!Touch::iterate(nex_listen_list, page, component, event)) {
        Nextion::sendLog(LogSeverity::Warning, "TouchEvent ignored");
    }
}

Nextion::Nextion(Stream &port, LogCallback logCb)
    : _port(port, logCb), _logCallback(logCb), _timer(_port), _gpio(_port) {
    _isHealthy = true;
    // response verbosity (1=OnSuccess 2=OnFailure 3=Always)
    _port.sendCommand("bkcmd=3");
    _isHealthy &= _port.recvRetCommandFinished();
    _port.sendCommand("page 0");
    _isHealthy &= _port.recvRetCommandFinished();
    if (!_isHealthy) {
        sendLog(nxt::LogSeverity::Error, "Init failed");
    }
}

void Nextion::sendLog(LogSeverity level, std::string log) {
    if (_logCallback != nullptr) {
        _logCallback(level, log);
    }
}
