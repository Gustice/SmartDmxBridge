/**
 * @file NexTouch.cpp
 *
 * The implementation of class NexTouch.
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
#include "bases/NexTouch.h"
#include <vector>

NexTouch::NexTouch(uint8_t pid, uint8_t cid, std::string_view name) : NexObject(pid, cid, name) {}

void NexTouch::attachPush(NexTouchEventCb push, void *ptr) {
    this->_pushCb = push;
    this->_ptrForPushCb = ptr;
}

void NexTouch::detachPush(void) {
    this->_pushCb = nullptr;
    this->_ptrForPushCb = nullptr;
}

void NexTouch::attachPop(NexTouchEventCb pop, void *ptr) {
    this->_popCb = pop;
    this->_ptrForPopCb = ptr;
}

void NexTouch::detachPop(void) {
    this->_popCb = nullptr;
    this->_ptrForPopCb = nullptr;
}

void NexTouch::push(void) {
    if (_pushCb) {
        _pushCb(_ptrForPushCb);
    }
}

void NexTouch::pop(void) {
    if (_popCb) {
        _popCb(_ptrForPopCb);
    }
}

void NexTouch::iterate(const SensingList & list, uint8_t pid, uint8_t cid, int32_t event) {
    for (auto &&e : list)
    {
        if (e->getObjPid() == pid && e->getObjCid() == cid) {
            e->printObjInfo();
            if (static_cast<int>(Event::Push) == event) {
                e->push();
            } else if (static_cast<int>(Event::Pop) == event) {
                e->pop();
            }
            break;
        }
    }
}
