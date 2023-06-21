/**
 * @file NexGauge.cpp
 *
 * The implementation of class NexGauge. 
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

#include "NexGauge.h"

NexGauge::NexGauge(uint8_t pid, uint8_t cid, const char *name)
    :NexObject(pid, cid, name)
{
}

bool NexGauge::getValue(uint32_t *number)
{
    std::string cmd("get ");
    cmd += getObjName();
    cmd += ".val";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexGauge::setValue(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".val=";
    cmd += buf;

    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexGauge::Get_background_color_bco(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".bco";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexGauge::Set_background_color_bco(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".bco=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd="";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexGauge::Get_font_color_pco(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".pco";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexGauge::Set_font_color_pco(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".pco=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexGauge::Get_pointer_thickness_wid(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".wid";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexGauge::Set_pointer_thickness_wid(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".wid=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexGauge::Get_background_cropi_picc(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".picc";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexGauge::Set_background_crop_picc(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".picc=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

 
