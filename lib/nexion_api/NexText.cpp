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

NexText::NexText(uint8_t pid, uint8_t cid, const char *name)
    :NexTouch(pid, cid, name)
{
}

uint16_t NexText::getText(char *buffer, uint16_t len)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".txt";
    sendCommand(cmd);
    return recvRetString(buffer,len);
}

bool NexText::setText(const char *buffer)
{
    std::string cmd;
    cmd += getObjName();
    cmd += ".txt=\"";
    cmd += buffer;
    cmd += "\"";
    sendCommand(cmd);
    return recvRetCommandFinished();    
}

uint32_t NexText::Get_background_color_bco(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".bco";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexText::Set_background_color_bco(uint32_t number)
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

uint32_t NexText::Get_font_color_pco(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".pco";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexText::Set_font_color_pco(uint32_t number)
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

uint32_t NexText::Get_place_xcen(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".xcen";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexText::Set_place_xcen(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".xcen=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexText::Get_place_ycen(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".ycen";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexText::Set_place_ycen(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".ycen=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexText::getFont(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".font";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexText::setFont(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".font=";
    cmd += buf;
    sendCommand(cmd);

    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexText::Get_background_crop_picc(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".picc";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexText::Set_background_crop_picc(uint32_t number)
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

uint32_t NexText::Get_background_image_pic(uint32_t *number)
{
    std::string cmd("get ");
    cmd += getObjName();
    cmd += ".pic";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexText::Set_background_image_pic(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".pic=";
    cmd += buf;

    sendCommand(cmd);
    return recvRetCommandFinished();
}




