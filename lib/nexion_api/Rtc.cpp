// /**
//  * @file Rtc.cpp
//  *
//  * The implementation of class Rtc.
//  *
//  * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
//  * @date    2015/8/13
//  * @copyright
//  * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
//  * This program is free software; you can redistribute it and/or
//  * modify it under the terms of the GNU General Public License as
//  * published by the Free Software Foundation; either version 2 of
//  * the License, or (at your option) any later version.
//  */
// #include "Rtc.hpp"
// #include "Nextion.hpp"
// #include "Touch.hpp"

// #include <cstring>

// namespace Nxt {

// bool Rtc::write_rtc_time(char *time) {
//     char year[5], mon[3], day[3], hour[3], min[3], sec[3];

//     if (strlen(time) < 19) {
//         return false;
//     }

//     year[0] = time[0];
//     year[1] = time[1];
//     year[2] = time[2];
//     year[3] = time[3];
//     year[4] = '\0';
//     mon[0] = time[5];
//     mon[1] = time[6];
//     mon[2] = '\0';
//     day[0] = time[8];
//     day[1] = time[9];
//     day[2] = '\0';
//     hour[0] = time[11];
//     hour[1] = time[12];
//     hour[2] = '\0';
//     min[0] = time[14];
//     min[1] = time[15];
//     min[2] = '\0';
//     sec[0] = time[17];
//     sec[1] = time[18];
//     sec[2] = '\0';

//     std::string cmd;
//     cmd = "rtc0=";
//     cmd += year;
//     NxtIo::sendCommand(cmd);
//     NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc1=";
//     cmd += mon;
//     NxtIo::sendCommand(cmd);
//     NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc2=";
//     cmd += day;
//     NxtIo::sendCommand(cmd);
//     NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc3=";
//     cmd += hour;
//     NxtIo::sendCommand(cmd);
//     NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc4=";
//     cmd += min;
//     NxtIo::sendCommand(cmd);
//     NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc5=";
//     cmd += sec;
//     NxtIo::sendCommand(cmd);
//     NxtIo::recvRetCommandFinished();

//     return true;
// }

// bool Rtc::write_rtc_time(uint32_t time[6]) {
//     char year[5], mon[3], day[3], hour[3], min[3], sec[3];
//     std::string cmd("rtc");
//     bool ret = true;

//     cmd += "0=";
//     cmd += std::to_string(time[0]);
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc1=";
//     cmd += std::to_string(time[1]);
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc2=";
//     cmd += std::to_string(time[2]);
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc3=";
//     cmd += std::to_string(time[3]);
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc4=";
//     cmd += std::to_string(time[4]);
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetCommandFinished();

//     cmd = "";
//     cmd += "rtc5=";
//     cmd += std::to_string(time[5]);
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetCommandFinished();
//     return ret;
// }

// bool Rtc::write_rtc_time(char *time_type, uint32_t number) {
//     std::string cmd("rtc");
//     if (strstr(time_type, "year")) {
//         cmd += "0=";
//         cmd += std::to_string(number);
//     }
//     if (strstr(time_type, "mon")) {
//         cmd += "1=";
//         cmd += std::to_string(number);
//     }
//     if (strstr(time_type, "day")) {
//         cmd += "2=";
//         cmd += std::to_string(number);
//     }
//     if (strstr(time_type, "hour")) {
//         cmd += "3=";
//         cmd += std::to_string(number);
//     }
//     if (strstr(time_type, "min")) {
//         cmd += "4=";
//         cmd += std::to_string(number);
//     }
//     if (strstr(time_type, "sec")) {
//         cmd += "5=";
//         cmd += std::to_string(number);
//     }

//     NxtIo::sendCommand(cmd);
//     return NxtIo::recvRetCommandFinished();
// }

// bool Rtc::read_rtc_time(char *time, uint32_t len) {
//     char time_buf[22] = {"0000/00/00 00:00:00 0"};
//     uint32_t year, mon, day, hour, min, sec, week;
//     std::string cmd;

//     cmd = "get rtc0";
//     NxtIo::sendCommand(cmd);
//     year = NxtIo::recvRetNumber();

//     cmd = "get rtc1";
//     NxtIo::sendCommand(cmd);
//     mon = NxtIo::recvRetNumber();

//     cmd = "get rtc2";
//     NxtIo::sendCommand(cmd);
//     day = NxtIo::recvRetNumber();

//     cmd = "get rtc3";
//     NxtIo::sendCommand(cmd);
//     hour = NxtIo::recvRetNumber();

//     cmd = "get rtc4";
//     NxtIo::sendCommand(cmd);
//     min = NxtIo::recvRetNumber();

//     cmd = "get rtc5";
//     NxtIo::sendCommand(cmd);
//     sec = NxtIo::recvRetNumber();

//     cmd = "get rtc6";
//     NxtIo::sendCommand(cmd);
//     week = NxtIo::recvRetNumber();

//     time_buf[0] = year / 1000 + '0';
//     time_buf[1] = (year / 100) % 10 + '0';
//     time_buf[2] = (year / 10) % 10 + '0';
//     time_buf[3] = year % 10 + '0';
//     time_buf[5] = mon / 10 + '0';
//     time_buf[6] = mon % 10 + '0';
//     time_buf[8] = day / 10 + '0';
//     time_buf[9] = day % 10 + '0';
//     time_buf[11] = hour / 10 + '0';
//     time_buf[12] = hour % 10 + '0';
//     time_buf[14] = min / 10 + '0';
//     time_buf[15] = min % 10 + '0';
//     time_buf[17] = sec / 10 + '0';
//     time_buf[18] = sec % 10 + '0';
//     time_buf[20] = week + '0';
//     time_buf[21] = '\0';

//     if (len >= 22) {
//         for (int i = 0; i < 22; i++) {
//             time[i] = time_buf[i];
//         }
//     } else {
//         for (int i = 0; i < len; i++) {
//             time[i] = time_buf[i];
//         }
//     }
//     return true;
// }

// bool Rtc::read_rtc_time(uint32_t *time, uint32_t len) {
//     uint32_t time_buf[7] = {0};
//     std::string cmd;
//     bool ret = true;

//     cmd = "get rtc0";
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetNumber(time_buf[0]);

//     cmd = "get rtc1";
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetNumber(time_buf[1]);

//     cmd = "get rtc2";
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetNumber(time_buf[2]);

//     cmd = "get rtc3";
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetNumber(time_buf[3]);

//     cmd = "get rtc4";
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetNumber(time_buf[4]);

//     cmd = "get rtc5";
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetNumber(time_buf[5]);

//     cmd = "get rtc6";
//     NxtIo::sendCommand(cmd);
//     ret &= NxtIo::recvRetNumber(time_buf[6]);

//     for (int i = 0; i < len; i++) {
//         time[i] = time_buf[i];
//     }
//     return ret;
// }

// uint32_t Rtc::read_rtc_time(std::string type) {
//     std::string cmd("get rtc");

//     if (type == "year") {
//         cmd += '0';
//     } else if (type == "mon") {
//         cmd += '1';
//     } else if (type == "day") {
//         cmd += '2';
//     } else if (type == "hour") {
//         cmd += '3';
//     } else if (type == "min") {
//         cmd += '4';
//     } else if (type == "sec") {
//         cmd += '5';
//     } else if (type == "week") {
//         cmd += '6';
//     } else {
//         return -1;
//     }

//     NxtIo::sendCommand(cmd);
//     return NxtIo::recvRetNumber();
// }
// } // namespace Nxt