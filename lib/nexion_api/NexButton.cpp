/**
 * @file NexButton.cpp
 *
 * The implementation of class NexButton. 
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

#include "NexButton.h"

NexButton::NexButton(uint8_t pid, uint8_t cid, const char *name)
    :NexTouch(pid, cid, name)
{
}

uint16_t NexButton::getText(char *buffer, uint16_t len)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".txt";
    sendCommand(cmd);
    return recvRetString(buffer,len);
}

bool NexButton::setText(const char *buffer)
{
    std::string cmd;
    cmd += getObjName();
    cmd += ".txt=\"";
    cmd += buffer;
    cmd += "\"";
    sendCommand(cmd);
    return recvRetCommandFinished();    
}


uint32_t NexButton::Get_background_color_bco(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".bco";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_background_color_bco(uint32_t number)
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

uint32_t NexButton::Get_press_background_color_bco2(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".bco2";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_press_background_color_bco2(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".bco2=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd="";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexButton::Get_font_color_pco(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".pco";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_font_color_pco(uint32_t number)
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

uint32_t NexButton::Get_press_font_color_pco2(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".pco2";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_press_font_color_pco2(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".pco2=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexButton::Get_place_xcen(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".xcen";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_place_xcen(uint32_t number)
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

uint32_t NexButton::Get_place_ycen(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".ycen";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_place_ycen(uint32_t number)
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

uint32_t NexButton::getFont(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".font";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::setFont(uint32_t number)
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

uint32_t NexButton::Get_background_cropi_picc(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".picc";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_background_crop_picc(uint32_t number)
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

uint32_t NexButton::Get_press_background_crop_picc2(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".picc2";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_press_background_crop_picc2(uint32_t number)
{
	char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".picc2=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexButton::Get_background_image_pic(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".pic";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_background_image_pic(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".pic=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexButton::Get_press_background_image_pic2(uint32_t *number)
{
    std::string cmd;
    cmd += "get ";
    cmd += getObjName();
    cmd += ".pic2";
    sendCommand(cmd);
    return recvRetNumber(number);
}

bool NexButton::Set_press_background_image_pic2(uint32_t number)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(number, buf, 10);
    cmd += getObjName();
    cmd += ".pic2=";
    cmd += buf;
    sendCommand(cmd);
	
    cmd = "";
    cmd += "ref ";
    cmd += getObjName();
    sendCommand(cmd);
    return recvRetCommandFinished();
}
