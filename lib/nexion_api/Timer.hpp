#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * Timer component.
 *
 * Commonly, you want to do something after set timer cycle and enable it,and the cycle value
 * must be greater than 50
 *
 */
class Timer : public NexTouch {
  public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string name);
     */
    Timer(uint8_t pid, uint8_t cid, std::string name) : NexTouch(pid, cid, name) {}

    /**
     * Attach an callback function of timer respond event.
     *
     * @param timer - callback called with ptr when a timer respond event occurs.
     * @param ptr - parameter passed into push[default:NULL].
     * @return none.
     *
     * @note If calling this method multiply, the last call is valid.
     */
    void attachTimer(NexTouchEventCb timer, void *ptr = NULL) {
        NexTouch::attachPop(timer, ptr);
    }

    /**
     * Detach an callback function.
     *
     * @return none.
     */
    void detachTimer(void) {
        NexTouch::detachPop();
    }

    /**
     * cycle attribute of component.
     */
    IntegerValue cycleTime{*this, "tim"};

    /**
     * contorl timer enable.
     *
     * @retval true - success.
     * @retval false - failed.
     */
    bool enable(void) {
        sendCommand(std::string{getObjName()} + ".en=1");
        return recvRetCommandFinished();
    }

    /**
     * contorl timer disable.
     *
     * @retval true - success.
     * @retval false - failed.
     */
    bool disable(void) {
        sendCommand(std::string{getObjName()} + ".en=0");
        return recvRetCommandFinished();
    }
};
/**
 * @}
 */

} // namespace Nxt
