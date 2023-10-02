#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief Timer component.
 *
 * @details Commonly, you want to do something after set timer cycle and enable it,and the cycle value
 * must be greater than 50
 */
class Timer : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Timer(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}

    /// @brief Attach one callback function of timer respond event
    /// @param timer callback called with ptr when a timer respond event occurs
    /// @param ptr parameter passed into push
    void attachTimer(Touch::eventCb timer, void *ptr = nullptr) {
        Touch::attachPop(timer, ptr);
    }

    /// @brief Detach an callback function
    void detachTimer() {
        Touch::detachPop();
    }

    /// @brief cycle attribute of component
    IntegerValue cycleTime{*this, "tim"};

    /// @brief Control timer enable
    /// @return success flag
    bool enable() {
        _port.sendCommand(std::string{getObjName()} + ".en=1");
        return _port.recvRetCommandFinished();
    }

    /// @brief Control timer disable
    /// @return success flag
    bool disable(void) {
        _port.sendCommand(std::string{getObjName()} + ".en=0");
        return _port.recvRetCommandFinished();
    }
};

} // namespace nxt
