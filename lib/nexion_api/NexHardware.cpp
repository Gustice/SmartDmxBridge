#include "NexHardware.h"
#include "Page.hpp"



constexpr uint8_t EndChar = 0xFF;
const std::string EndSequence = "\xFF\xFF\xFF";

/*
 * Receive uint32_t data.
 *
 * @param number - save uint32_t data.
 * @param timeout - set timeout time.
 *
 * @retval true - success.
 * @retval false - failed.
 *
 */
uint32_t NxtIo::recvRetNumber(uint32_t timeout) {
    constexpr unsigned RS = 5;
    auto read = readCode(RS, timeout);
    if (RS != read.size()) {
        NxtIo::sendLog(NxtLogSeverity::Warning, "recvRetNumber timeout");
        return false;
    }

    if (static_cast<NxtIo::Return>(read[0]) == NxtIo::Return::NumericDataEnclosed) {
        auto number =
            ((uint32_t)read[4] << 24) | ((uint32_t)read[3] << 16) | (read[2] << 8) | (read[1]);
        NxtIo::sendLog(NxtLogSeverity::Debug, "recvRetNumber :" + std::to_string(number));
        return number;
    }

    NxtIo::sendLog(NxtLogSeverity::Warning, "recvRetNumber err");
    return 0;
}

static std::queue<NxtIo::Buffer> interruptQueue;

NxtIo::Buffer NxtIo::readCode(size_t payload, unsigned msTimeout) {
    constexpr unsigned D = 3;
    auto minSize = payload + D;
    Buffer readNext;
    Buffer read;

    if (interruptQueue.size() > 0) {
        auto last = interruptQueue.front();
        if (last.size() == minSize) {
            interruptQueue.pop();
            return last;
        }
    }

    while (read.size() != payload) {
        Buffer buf(readNext.begin(), readNext.end());
        {
            auto raw = serialPort->read(minSize, msTimeout);
            for (auto &&e : raw) {
                buf.push_back(e);
            }
        }
        if (buf.size() < minSize) {
            return read;
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

        readNext = Buffer(buf.begin() + idx, buf.end());
        if (read.size() != payload) {
            interruptQueue.push(read);
        }
        minSize = payload + D - readNext.size();
    }

    return read;
}

NxtIo::Buffer NxtIo::readString(unsigned msTimeout) {
    constexpr unsigned D = 3;
    Buffer read;
    int delCnt = D;
    int idx = 0;

    while (delCnt > 0) {
        auto raw = serialPort->read(D, msTimeout);
        if (raw.size() == 0) {
            return read;
        }
        for (auto &&e : raw) {
            read.push_back(e);
        }

        while (delCnt > 0 && idx < read.size()) {
            if (read[idx] != EndChar) {
                read.push_back(read[idx]);
            } else {
                delCnt--;
            }
            idx++;
        }
    }

    return read;
}

/*
 * Receive string data.
 *
 * @param buffer - save string data.
 * @param len - string buffer length.
 * @param timeout - set timeout time.
 *
 * @return the length of string buffer.
 *
 */
std::string NxtIo::recvRetString(uint32_t timeout) {
    static std::string lastRead;
    bool str_start_flag = false;
    static uint8_t cnt_0xff = 0;
    std::string temp = lastRead;
    lastRead = "";

    auto read = readString(timeout);
    for (auto &&c : read) {
        if (str_start_flag) {
            if (0xFF == c) {
                cnt_0xff++;
                if (cnt_0xff >= 3) {
                    break;
                }
            } else {
                temp += (char)c;
            }
        } else if (static_cast<NxtIo::Return>(c) == NxtIo::Return::StringDataEnclosed) {
            str_start_flag = true;
        }
    }

    if (cnt_0xff < 3) {
        lastRead += temp;
        return {};
    }
    cnt_0xff = 0;
    return temp;
}

/*
 * Send command to Nextion.
 *
 * @param cmd - the string of command.
 */
void NxtIo::sendCommand(std::string cmd) {
    cmd += "\xFF\xFF\xFF";
    serialPort->write(cmd);
}

/*
 * Command is executed successfully.
 *
 * @param timeout - set timeout time.
 *
 * @retval true - success.
 * @retval false - failed.
 *
 */
bool NxtIo::recvRetCommandFinished(uint32_t timeout) {
    constexpr unsigned AS = 1;
    auto read = readCode(AS, timeout);
    if (AS != read.size()) {
        NxtIo::sendLog(NxtLogSeverity::Warning, "recvRetCommandFinished timeout");
        return false;
    }

    if (static_cast<NxtIo::Return>(read[0]) == NxtIo::Return::InstructionSuccessful) {
        NxtIo::sendLog(NxtLogSeverity::Debug, "recvRetCommandFinished ok");
        return true;
    }

    NxtIo::sendLog(NxtLogSeverity::Warning, "recvRetCommandFinished err");
    return false;
}

bool NxtIo::nexInit(SerialStream &port, NxtIo::LogCallback logCb) {
    serialPort = &port;
    logCallback = logCb;

    bool ret = true;
    // response verbosity (1=OnSuccess 2=OnFailure 3=Always)
    sendCommand("bkcmd=3");
    ret &= recvRetCommandFinished();
    sendCommand("page 0");
    ret &= recvRetCommandFinished();
    return ret;
}

void NxtIo::nexLoop(const SensingList &nex_listen_list) {
    constexpr unsigned ES = 4;
    auto input = readCode(ES, 0);
    if (input.size() != ES)
        return;

    if (static_cast<NxtIo::Return>(input[0]) == NxtIo::Return::TouchEvent) {
        uint8_t page = input[1];
        uint8_t component = input[2];
        int32_t event = (int32_t)input[3];

        NxtIo::sendLog(NxtLogSeverity::Debug, std::string{"TouchEvent: "} +
                                                  std::to_string(static_cast<int>(input[1])) + " " +
                                                  std::to_string(static_cast<int>(input[2])) + " " +
                                                  std::to_string(static_cast<int>(input[3])));

        if (!Nxt::Touch::iterate(nex_listen_list, page, component, event)) {
            NxtIo::sendLog(NxtLogSeverity::Warning, "TouchEvent ignored");
        }
    }
}

void NxtIo::sendLog(NxtLogSeverity level, std::string log) {
    if (logCallback != nullptr) {
        logCallback(level, log);
    }
}

NxtIo::LogCallback NxtIo::logCallback;
SerialStream *NxtIo::serialPort;
