/**
 * @file NexSlider.cpp
 *
 * The implementation of class NexSlider. 
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
#include "NexSlider.h"

NexSlider::NexSlider(uint8_t pid, uint8_t cid, const char *name)
    :NexTouch(pid, cid, name)
{
}

uint32_t NexSlider::getValue() {
    sendCommand(std::string{"get "} + getObjName() + ".val");
    return recvRetNumber();
}

bool NexSlider::setValue(uint32_t number) {
    sendCommand(std::string{getObjName()} + ".val=" + std::to_string(number));
    return recvRetCommandFinished();
}

uint32_t NexSlider::Get_pointer_thickness_wid() {
    sendCommand(std::string{"get "} + getObjName() + ".wid");
    return recvRetNumber();
}

bool NexSlider::Set_pointer_thickness_wid(uint32_t number) {
    sendCommand(std::string{getObjName()} + ".wid=" + std::to_string(number));
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexSlider::Get_cursor_height_hig() {
    sendCommand(std::string{"get "} + getObjName() + ".hig");
    return recvRetNumber();
}

bool NexSlider::Set_cursor_height_hig(uint32_t number) {
    sendCommand(std::string{getObjName()} + ".hig=" + std::to_string(number));
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexSlider::getMaxval() {
    sendCommand(std::string{"get "} + getObjName() + ".maxval");
    return recvRetNumber();
}

bool NexSlider::setMaxval(uint32_t number) {
    sendCommand(std::string{getObjName()} + ".maxval=" + std::to_string(number));
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}

uint32_t NexSlider::getMinval() {
    sendCommand(std::string{"get "} + getObjName() + ".minval");
    return recvRetNumber();
}

bool NexSlider::setMinval(uint32_t number) {
    sendCommand(std::string{getObjName()} + ".minval=" + std::to_string(number));
    // cmd = "";
    // cmd += "ref ";
    // cmd += getObjName();
    // sendCommand(cmd);
    // return recvRetCommandFinished();
}