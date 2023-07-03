#include "NexHardware.h"
#include "Page.hpp"

enum class NxtReturn {
    CMD_FINISHED = 0x01,
    EVENT_LAUNCHED = 0x88,
    EVENT_UPGRADED = 0x89,
    EVENT_TOUCH_HEAD = 0x65,
    EVENT_POSITION_HEAD = 0x67,
    EVENT_SLEEP_POSITION_HEAD = 0x68,
    CURRENT_PAGE_ID_HEAD = 0x66,
    STRING_HEAD = 0x70,
    NUMBER_HEAD = 0x71,
    INVALID_CMD = 0x00,
    INVALID_COMPONENT_ID = 0x02,
    INVALID_PAGE_ID = 0x03,
    INVALID_PICTURE_ID = 0x04,
    INVALID_FONT_ID = 0x05,
    INVALID_BAUD = 0x11,
    INVALID_VARIABLE = 0x1A,
    INVALID_OPERATION = 0x1B,
};

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
uint32_t NxtIo::recvRetNumber(uint32_t timeout)
{
    auto read = serialPort->read(8, timeout);
    if (8 != read.size()) {
        NxtIo::sendLog("recvRetNumber err");
        return false;
    }

    if ( static_cast<NxtReturn>(read[0]) == NxtReturn::NUMBER_HEAD
        && read[5] == 0xFF
        && read[6] == 0xFF
        && read[7] == 0xFF
        ) {
        auto number = ((uint32_t)read[4] << 24) | ((uint32_t)read[3] << 16) | (read[2] << 8) | (read[1]);
        NxtIo::sendLog("recvRetNumber :" + std::to_string(number));
        return number;
    }

    NxtIo::sendLog("recvRetNumber err");
    return 0;
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
std::string NxtIo::recvRetString(uint32_t timeout)
{
    static std::string lastRead;
    bool str_start_flag = false;
    static uint8_t cnt_0xff = 0;
    std::string temp = lastRead;
    lastRead = "";
    
    auto read = serialPort->read(3, timeout);
    for (auto &&c : read)
    {
        if (str_start_flag) {
            if (0xFF == c) {
                cnt_0xff++;
                if (cnt_0xff >= 3) {
                    break;
                }
            }
            else {
                temp += (char)c;
            }
        }
        else if (static_cast<NxtReturn>(c) == NxtReturn::STRING_HEAD)
        {
            str_start_flag = true;
        }
    }

    if (cnt_0xff < 3)
    {   lastRead += temp;
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
void NxtIo::sendCommand(std::string cmd)
{
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
bool NxtIo::recvRetCommandFinished(uint32_t timeout)
{    
    auto read = serialPort->read(4, timeout);
    if (4 != read.size()) {
        return false;
    }

    if (static_cast<NxtReturn>(read[0]) == NxtReturn::CMD_FINISHED
        && read[1] == 0xFF
        && read[2] == 0xFF
        && read[3] == 0xFF
        )
    {
        NxtIo::sendLog("recvRetCommandFinished ok");
        return true;
    }

    NxtIo::sendLog("recvRetCommandFinished err");
    return false;
}


bool NxtIo::nexInit(SerialStream &port, NxtIo::LogCallback logCb)
{
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

void NxtIo::nexLoop(const SensingList & nex_listen_list)
{
    auto input = serialPort->read(7,0);
    if (input.size() > 0)
    {
        if (static_cast<NxtReturn>(input[0]) == NxtReturn::EVENT_TOUCH_HEAD)
        {
            if (input.size() >= 7)
            {
                if (0xFF == (input[4] & input[5] & input[6])) {
                    Nxt::Touch::iterate(nex_listen_list, input[1], input[2], (int32_t)input[3]);
                }
            }
        }
    }
}


void NxtIo::sendLog(std::string log) {
    if (logCallback != nullptr) {
        logCallback(log);
    }
}

NxtIo::LogCallback NxtIo::logCallback;
SerialStream * NxtIo::serialPort;

