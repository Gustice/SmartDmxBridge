/**
 * @file NexHardware.cpp
 *
 * The implementation of base API for using Nextion. 
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/8/11
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include "NexHardware.h"
#include <cstring>

// enum class NextReturn {
// }

#define NEX_RET_CMD_FINISHED            (0x01)
#define NEX_RET_EVENT_LAUNCHED          (0x88)
#define NEX_RET_EVENT_UPGRADED          (0x89)
#define NEX_RET_EVENT_TOUCH_HEAD            (0x65)     
#define NEX_RET_EVENT_POSITION_HEAD         (0x67)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD   (0x68)
#define NEX_RET_CURRENT_PAGE_ID_HEAD        (0x66)
#define NEX_RET_STRING_HEAD                 (0x70)
#define NEX_RET_NUMBER_HEAD                 (0x71)
#define NEX_RET_INVALID_CMD             (0x00)
#define NEX_RET_INVALID_COMPONENT_ID    (0x02)
#define NEX_RET_INVALID_PAGE_ID         (0x03)
#define NEX_RET_INVALID_PICTURE_ID      (0x04)
#define NEX_RET_INVALID_FONT_ID         (0x05)
#define NEX_RET_INVALID_BAUD            (0x11)
#define NEX_RET_INVALID_VARIABLE        (0x1A)
#define NEX_RET_INVALID_OPERATION       (0x1B)

static CharStream * SerialPort = nullptr;
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
uint32_t recvRetNumber(uint32_t timeout)
{
    // if (!number) {
    //     dbSerialPrintln("recvRetNumber err");
    //     return false;
    // }
    
    // auto buf = SerialPort->readBytes(8);
    // if (8 != buf.size()) {
    //     dbSerialPrintln("recvRetNumber err");
    //     return false;
    // }

    // if (buf[0] == NEX_RET_NUMBER_HEAD
    //     && buf[5] == 0xFF
    //     && buf[6] == 0xFF
    //     && buf[7] == 0xFF
    //     ) {
    //     *number = ((uint32_t)buf[4] << 24) | ((uint32_t)buf[3] << 16) | (buf[2] << 8) | (buf[1]);
    //     dbSerialPrint("recvRetNumber :");
    //     dbSerialPrintln(*number);
    //     return true;
    // }

    // dbSerialPrintln("recvRetNumber err");
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
std::string recvRetString(uint32_t timeout)
{
    uint16_t ret = 0;
//     bool str_start_flag = false;
//     uint8_t cnt_0xff = 0;
//     std::string temp;
//     uint8_t c = 0;
//     long start;

//     if (!buffer || len == 0) {
//         return 0;
//     }
    
//     auto read = SerialPort->read();
    
//     while (millis() - start <= timeout)
//     {
//         while (nexSerial.available())
//         {
//             c = nexSerial.read();
//             if (str_start_flag)
//             {
//                 if (0xFF == c)
//                 {
//                     cnt_0xff++;                    
//                     if (cnt_0xff >= 3)
//                     {
//                         break;
//                     }
//                 }
//                 else
//                 {
//                     temp += (char)c;
//                 }
//             }
//             else if (NEX_RET_STRING_HEAD == c)
//             {
//                 str_start_flag = true;
//             }
//         }
        
//         if (cnt_0xff >= 3)
//         {
//             break;
//         }
//     }

//     ret = temp.length();
//     ret = ret > len ? len : ret;
//     strncpy(buffer, temp.c_str(), ret);

    return "";
}

/*
 * Send command to Nextion.
 *
 * @param cmd - the string of command.
 */
void sendCommand(std::string cmd)
{
    cmd += "\xFF\xFF\xFF";
    SerialPort->write(cmd);
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
bool recvRetCommandFinished(uint32_t timeout)
{    
    // bool ret = false;
    // uint8_t temp[4] = {0};
    
    // auto buf = SerialPort->readBytes(4, timeout);
    // if (4 != buf.size()) {
    //     ret = false;
    // }

    // if (temp[0] == NEX_RET_CMD_FINISHED
    //     && temp[1] == 0xFF
    //     && temp[2] == 0xFF
    //     && temp[3] == 0xFF
    //     )
    // {
    //     dbSerialPrintln("recvRetCommandFinished ok");
    //     ret = true;
    // }

    // dbSerialPrintln("recvRetCommandFinished err");
    return false;
}


bool nexInit(CharStream &port)
{
    SerialPort = &port;
    bool ret = true;
    
    // sendCommand("bkcmd=1"); // response verbosity (1=OnSuccess 2=OnFailure 3=Always)
    // ret &= recvRetCommandFinished();
    // sendCommand("page 0");
    // ret &= recvRetCommandFinished();
    return ret;
}

void nexLoop(NexTouch *nex_listen_list[])
{
    // uint16_t i;
    // uint8_t c;  
    
    // auto input = SerialPort->readBytes(6);

    // if (input.size() > 0)
    // {
    //     if (NEX_RET_EVENT_TOUCH_HEAD == input[0])
    //     {
    //         if (input.size() >= 6)
    //         {
    //             if (0xFF == (input[4] & input[5] & input[6])) {
    //                 NexTouch::iterate(nex_listen_list, input[1], input[2], (int32_t)input[3]);
    //             }
    //         }
    //     }
    // }
    

}
