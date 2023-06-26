/**
 * @file NexText.cpp
 *
 * The implementation of class NexText. 
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
#include "NexText.h"

NexText::NexText(uint8_t pid, uint8_t cid, std::string name)
    :NexTouch(pid, cid, name)
{
}

std::string NexText::getText()
{
    sendCommand(std::string{"get "} + getObjName() + ".txt");
    return recvRetString();
}

bool NexText::setText(std::string value)
{
    sendCommand(std::string {getObjName()} + ".txt=\""+ value + "\"");
    return recvRetCommandFinished();
}
