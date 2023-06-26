/**
 * @file NexScrolltext.cpp
 *
 * The implementation of class NexScrolltext. 
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/8/13
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include "NexScrolltext.h"

NexScrolltext::NexScrolltext(uint8_t pid, uint8_t cid, const char *name)
    :NexTouch(pid, cid, name)
{
}

std::string NexScrolltext::getText()
{
    sendCommand(std::string{"get "} + getObjName() + ".txt");
    return recvRetString();
}

bool NexScrolltext::setText(std::string value)
{
    sendCommand(std::string{getObjName()} + ".txt=\""+ value + "\"");
    return recvRetCommandFinished();    
}

uint32_t NexScrolltext::Get_scroll_dir()
{
    sendCommand(std::string{"get "} + getObjName() + ".dir");
    return recvRetNumber();
}

bool NexScrolltext::Set_scroll_dir(uint32_t number)
{
    sendCommand(std::string{getObjName()} + ".dir=" + std::to_string(number));
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexScrolltext::Get_scroll_distance()
{
    sendCommand(std::string{"get "} + getObjName() + ".dis");
    return recvRetNumber();
}

bool NexScrolltext::Set_scroll_distance(uint32_t number)
{
    if (number < 2) {
        number = 2;
    }
    sendCommand(std::string{getObjName()} + ".dis=" + std::to_string(number));
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexScrolltext::Get_cycle_tim()
{
    sendCommand(std::string{"get "} + getObjName() + ".tim");
    return recvRetNumber();
}

bool NexScrolltext::Set_cycle_tim(uint32_t number)
{
    if (number < 8) {
        number = 8;
    }
    sendCommand(std::string{getObjName()} + ".tim=" + std::to_string(number));
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

bool NexScrolltext::enable(void)
{
    sendCommand(std::string{getObjName()} + ".en=1");
    return recvRetCommandFinished();
}

bool NexScrolltext::disable(void)
{
    sendCommand(std::string{getObjName()} + ".en=0");
    return recvRetCommandFinished();
}
