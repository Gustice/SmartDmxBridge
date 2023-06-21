/**
 * @file NexGpio.cpp
 *
 * The implementation of class NexGpio. 
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
#include "NexGpio.h"

bool NexGpio::pin_mode(uint32_t port,uint32_t mode,uint32_t control_id)
{
    char buf;
    std::string cmd;
    
    cmd += "cfgpio ";
    buf = port + '0';
    cmd += buf;
    cmd += ',';
    buf = mode + '0';
    cmd += buf;
    cmd += ',';
    buf = control_id = '0';
    cmd += buf;

    sendCommand(cmd);
    return recvRetCommandFinished();
    
}

bool NexGpio::digital_write(uint32_t port,uint32_t value)
{
    std::string cmd;
    char buf;
    
    cmd += "pio";
    buf = port + '0';
    cmd += buf;
    cmd += '=';
    buf = value + '0';
    cmd += buf;
    
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t NexGpio::digital_read(uint32_t port)
{
    uint32_t number;
    char buf;
    
    std::string cmd("get ");
    cmd += "pio";
    buf = port + '0';
    cmd += buf;
    
    sendCommand(cmd);
    recvRetNumber(&number);
    return number;
}

bool NexGpio::analog_write(uint32_t port,uint32_t value)
{
    char buf[10] = {0};
    char c;
    std::string cmd;
    
    utoa(value, buf, 10);
    cmd += "pwm";
    c = port + '0';
    cmd += c;
    cmd += '=';
    cmd += buf;
    
    sendCommand(cmd);
    return recvRetCommandFinished();   
}

bool NexGpio::set_pwmfreq(uint32_t value)
{
    char buf[10] = {0};
    std::string cmd;
    
    utoa(value, buf, 10);
    cmd += "pwmf";
    cmd += '=';
    cmd += buf;
    
    sendCommand(cmd);
    return recvRetCommandFinished();   
}

uint32_t NexGpio::get_pwmfreq(uint32_t *number)
{
    std::string cmd("get pwmf");
    sendCommand(cmd);
    return recvRetNumber(number);
}