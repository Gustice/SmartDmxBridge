/**
 * @file NexPage.h
 *
 * The definition of class NexPage.
 *
 * @author Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date 2015/8/13
 *
 * @copyright
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#pragma once

#include "NexHardware.h"
#include "bases/NexTouch.h"
/**
 * @addtogroup Component
 * @{
 */

/**
 * A special component , which can contain other components such as NexButton,
 * NexText and NexWaveform, etc.
 */
class NexPage : public NexTouch {
  public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string name);
     */
    NexPage(uint8_t pid, uint8_t cid, std::string name) : NexTouch(pid, cid, name) {}

    /**
     * Show itself.
     *
     * @return true if success, false for faileure.
     */
    bool show(void) {
        sendCommand(std::string("page ") + getObjName());
        return recvRetCommandFinished();
    }
};
/**
 * @}
 */
