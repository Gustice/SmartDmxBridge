/**
 * @file NexTouch.h
 *
 * The definition of class NexTouch.
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

#ifndef __NEXTOUCH_H__
#define __NEXTOUCH_H__

#include "NexIncludes.h"
#include "NexObject.h"
#include <vector>

/**
 * @addtogroup TouchEvent
 * @{
 */

/**
 * Type of callback funciton when an touch event occurs.
 *
 * @param ptr - user pointer for any purpose. Commonly, it is a pointer to a object.
 * @return none.
 */
typedef void (*NexTouchEventCb)(void *ptr);

/**
 * Father class of the components with touch events.
 *
 * Derives from NexObject and provides methods allowing user to attach
 * (or detach) a callback function called when push(or pop) touch event occurs.
 */
class NexTouch : public NexObject {
  public:
    enum class Event {
        Pop = 0,  ///< Pop touch event occurring when your finger or pen leaving from Nextion touch
                  ///< panel.
        Push = 1, ///< Push touch event occurring when your finger or pen coming to Nextion touch
                  ///< panel.
    };

    using SensingList = std::vector<NexTouch *>;
    static void iterate(const SensingList &nex_listen_list, uint8_t pid, uint8_t cid,
                        int32_t event);

    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    NexTouch(uint8_t pid, uint8_t cid, std::string_view name);

    /**
     * Attach an callback function of push touch event.
     *
     * @param push - callback called with ptr when a push touch event occurs.
     * @param ptr - parameter passed into push[default:NULL].
     * @return none.
     *
     * @note If calling this method multiply, the last call is valid.
     */
    void attachPush(NexTouchEventCb push, void *ptr = nullptr);

    /**
     * Detach an callback function.
     *
     * @return none.
     */
    void detachPush(void);

    /**
     * Attach an callback function of pop touch event.
     *
     * @param pop - callback called with ptr when a pop touch event occurs.
     * @param ptr - parameter passed into pop[default:NULL].
     * @return none.
     *
     * @note If calling this method multiply, the last call is valid.
     */
    void attachPop(NexTouchEventCb pop, void *ptr = nullptr);

    /**
     * Detach an callback function.
     *
     * @return none.
     */
    void detachPop(void);

  private: /* methods */
    void push(void);
    void pop(void);

  private: /* data */
    NexTouchEventCb _pushCb = nullptr;
    void *_ptrForPushCb = nullptr;
    NexTouchEventCb _popCb = nullptr;
    void *_ptrForPopCb = nullptr;
};

/**
 * @}
 */

#endif /* #ifndef __NEXTOUCH_H__ */
