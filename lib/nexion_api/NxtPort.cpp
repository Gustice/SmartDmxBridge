
#include "NxtPort.hpp"

namespace nxt {

constexpr uint8_t EndChar = 0xFF;
const std::string EndSequence{"\xFF\xFF\xFF"};

constexpr std::array<uint8_t, UINT8_MAX> createBinning() {
    std::array<uint8_t, UINT8_MAX> bins{0xFF};
    /* InvalidInstruction */ bins[0x00] = static_cast<uint8_t>(CodeBin::Return);
    /* InstructionSuccessful */ bins[0x01] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidComponentID */ bins[0x02] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidPageID */ bins[0x03] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidPictureID */ bins[0x04] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidFontID */ bins[0x05] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidFileOperation */ bins[0x06] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidCRC */ bins[0x09] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidBaudrateSetting */ bins[0x11] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidWaveformID */ bins[0x12] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidVariableName */ bins[0x1A] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidVariableOperation */ bins[0x1B] = static_cast<uint8_t>(CodeBin::Return);
    /* Assignmentfailed */ bins[0x1C] = static_cast<uint8_t>(CodeBin::Return);
    /* EEPROMOperationFailed */ bins[0x1D] = static_cast<uint8_t>(CodeBin::Return);
    /* InvalidQuantityOfParameters */ bins[0x1E] = static_cast<uint8_t>(CodeBin::Return);
    /* IOOperationFailed */ bins[0x1F] = static_cast<uint8_t>(CodeBin::Return);
    /* EscapeCharacterInvalid */ bins[0x20] = static_cast<uint8_t>(CodeBin::Return);
    /* VariablenNmeTooLong */ bins[0x23] = static_cast<uint8_t>(CodeBin::Return);
    /* SerialBufferOverflow */ bins[0x24] = static_cast<uint8_t>(CodeBin::Return);

    /* TouchEvent */ bins[0x65] = static_cast<uint8_t>(CodeBin::Touch);
    /* NumericDataEnclosed */ bins[0x71] = static_cast<uint8_t>(CodeBin::Number);
    /* StringDataEnclosed */ bins[0x70] = static_cast<uint8_t>(CodeBin::String);
    /* CurrentPageNumber */ bins[0x66] = static_cast<uint8_t>(CodeBin::Trash);
    /* TouchCoordinateAwake */ bins[0x67] = static_cast<uint8_t>(CodeBin::Trash);
    /* TouchCoordinateSleep */ bins[0x68] = static_cast<uint8_t>(CodeBin::Trash);
    /* NextionStartup */ bins[0x00] = static_cast<uint8_t>(CodeBin::Trash);
    /* AutoEnteredSleepMode */ bins[0x86] = static_cast<uint8_t>(CodeBin::Trash);
    /* AutoWakeFromSleep */ bins[0x87] = static_cast<uint8_t>(CodeBin::Trash);
    /* NextionReady */ bins[0x88] = static_cast<uint8_t>(CodeBin::Trash);
    /* StartmicroSDUpgrade */ bins[0x89] = static_cast<uint8_t>(CodeBin::Trash);
    /* TransparentDataFinished */ bins[0xFD] = static_cast<uint8_t>(CodeBin::Trash);
    /* TransparentDataReady */ bins[0xFE] = static_cast<uint8_t>(CodeBin::Trash);

    return bins;
};
constexpr std::array<uint8_t, UINT8_MAX> CodeBinIndexes = createBinning();

Buffer Port::getFromBin(CodeBin code) {
    auto &bin = bins[static_cast<unsigned>(code)];
    if (bin.size() == 0)
        return {};

    auto f = bin.front();
    bin.pop();
    return f;
}

void Port::putInBin(Buffer payload) {
    Return code = static_cast<Return>(payload[0]);
    auto bI = CodeBinIndexes[static_cast<unsigned>(code)];
    auto &bin = bins[bI];
    bin.push(payload);
}

Buffer Port::readCode(CodeBin code, unsigned msTimeout) {
    constexpr unsigned D = 3;
    auto minSize = 1 + D;
    Buffer readNext;
    Buffer read;

    int retry = 2;

    while (retry-- > 0) {
        auto b = getFromBin(code);
        if (b.size() > 0) {
            return b;
        }

        Buffer buf(_remainder.begin(), _remainder.end());
        _remainder = {};
        {
            auto raw = _serialPort.read(minSize, msTimeout);
            for (auto &&e : raw) {
                buf.push_back(e);
            }
        }
        if (buf.size() == 0) {
            return {};
        }

        int delCnt = D;
        int idx = 0;

        while (delCnt > 0 && idx < buf.size()) {
            if (buf[idx] != EndChar) {
                read.push_back(buf[idx]);
            } else {
                delCnt--;
            }
            idx++;
        }

        if (delCnt == 0) {
            putInBin(read);
        } else {
            _remainder = Buffer(buf.begin(), buf.end());
        }
    }
    return {};
}

void Port::sendCommand(std::string payload) {
    payload += EndSequence.data();
    _serialPort.write(payload);
}

std::string Port::recvRetString(uint32_t timeout) {
    auto buffer = readCode(CodeBin::String, timeout);

    if (buffer.size() < 1) {
        Port::sendLog(LogSeverity::Warning, "recvRetString timeout");
        return "";
    }

    if (static_cast<Return>(buffer[0]) != Return::StringDataEnclosed) {
        Port::sendLog(LogSeverity::Warning, "recvRetString err");
        return "";
    }

    std::string read;
    for (auto &&c : buffer) {
        read += (char)c;
    }

    return read;
}

uint32_t Port::recvRetNumber(uint32_t timeout) {
    constexpr unsigned RS = 5;
    auto read = readCode(CodeBin::Number, timeout);
    if (RS != read.size()) {
        Port::sendLog(LogSeverity::Warning, "recvRetNumber timeout");
        return 0;
    }

    if (static_cast<Return>(read[0]) == Return::NumericDataEnclosed) {
        Port::sendLog(LogSeverity::Warning, "recvRetNumber err");
        return 0;
    }

    auto number = ((uint32_t)read[4] << 24) | ((uint32_t)read[3] << 16) | (read[2] << 8) | (read[1]);
    Port::sendLog(LogSeverity::Debug, "recvRetNumber :" + std::to_string(number));
    return number;
}

bool Port::recvRetCommandFinished(uint32_t timeout) {
    constexpr unsigned AS = 1;
    auto read = readCode(CodeBin::Return, timeout);
    if (AS != read.size()) {
        Port::sendLog(LogSeverity::Warning, "recvRetCommandFinished timeout");
        return false;
    }

    if (static_cast<Return>(read[0]) != Return::InstructionSuccessful) {
        Port::sendLog(LogSeverity::Warning, "recvRetCommandFinished err");
        return false;
    }

    Port::sendLog(LogSeverity::Debug, "recvRetCommandFinished ok");
    return true;
}


void Port::sendLog(nxt::LogSeverity level, std::string log) {
    if (_logCallback != nullptr) {
        _logCallback(level, log);
    }
}

} // namespace nxt
