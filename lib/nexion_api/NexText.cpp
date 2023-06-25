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

uint32_t NexText::Get_background_color_bco(uint32_t *number)
{
    sendCommand(std::string{"get "} + getObjName() + ".bco");
    return recvRetNumber(number);
}

bool NexText::Set_background_color_bco(uint32_t number)
{
    sendCommand(getObjName() + ".bco=" + std::to_string(number));
	
    // only if version is lower than 0.38
    // std::string refreshCmd;
    // refreshCmd += "ref ";
    // refreshCmd += getObjName();
    // sendCommand(refreshCmd);
    // return recvRetCommandFinished();
}

uint32_t NexText::Get_font_color_pco(uint32_t *number)
{
    sendCommand(std::string{"get "} + getObjName() + ".pco" );
    return recvRetNumber(number);
}

bool NexText::Set_font_color_pco(uint32_t number)
{
    sendCommand(getObjName() + ".pco=" + std::to_string(number));
	
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexText::Get_place_xcen(uint32_t *number)
{
    sendCommand(std::string{"get "} + getObjName() + ".xcen");
    return recvRetNumber(number);
}

bool NexText::Set_place_xcen(uint32_t number)
{
    sendCommand(getObjName() + ".xcen=" + std::to_string(number));
	
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexText::Get_place_ycen(uint32_t *number)
{
    sendCommand(std::string{"get "} + getObjName() + ".ycen");
    return recvRetNumber(number);
}

bool NexText::Set_place_ycen(uint32_t number)
{
    sendCommand(getObjName() + ".ycen=" + std::to_string(number));
	
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexText::getFont(uint32_t *number)
{
    sendCommand(std::string{"get "} + getObjName() + ".font");
    return recvRetNumber(number);
}

bool NexText::setFont(uint32_t number)
{
    sendCommand(getObjName() + ".font=" + std::to_string(number));

    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexText::Get_background_crop_picc(uint32_t *number)
{
    sendCommand(std::string{"get "} + getObjName() + ".picc");
    return recvRetNumber(number);
}

bool NexText::Set_background_crop_picc(uint32_t number)
{
    sendCommand(getObjName() + ".picc=" + std::to_string(number));
	
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexText::Get_background_image_pic(uint32_t *number)
{
    sendCommand(std::string{"get "} + getObjName() + ".pic");
    return recvRetNumber(number);
}

bool NexText::Set_background_image_pic(uint32_t number)
{
    sendCommand(getObjName() + ".pic=" + std::to_string(number));
    return recvRetCommandFinished();
}
