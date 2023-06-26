/**
 * @file NexNumber.cpp
 *
 * The implementation of class NexNumber. 
 *
 * @author  huang xianming (email:<xianming.huang@itead.cc>)
 * @date    2015/8/13
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include "NexNumber.h"

NexNumber::NexNumber(uint8_t pid, uint8_t cid, const char *name)
    :NexTouch(pid, cid, name)
{
}

bool NexNumber::getValue()
{
    sendCommand(std::string{"get "} + getObjName() + ".val");
    return recvRetNumber();
}

bool NexNumber::setValue(uint32_t number)
{
    sendCommand(std::string{getObjName()} + ".val=" + std::to_string(number));
    return recvRetCommandFinished();
}

uint32_t NexNumber::Get_number_lenth()
{
    sendCommand(std::string{"get "} + getObjName() + ".lenth");
    return recvRetNumber();
}

bool NexNumber::Set_number_lenth(uint32_t number)
{
    sendCommand(std::string{getObjName()} + ".lenth=" + std::to_string(number));
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}
