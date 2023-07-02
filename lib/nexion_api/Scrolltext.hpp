#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"
#include "bases/NexTouch.h"

namespace Nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * ScrollText component.
 */
class Scrolltext : public NexTouch {
  public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Scrolltext(uint8_t pid, uint8_t cid, std::string_view name) : NexTouch(pid, cid, name) {}

    /**
     * Text attribute of component.
     */
    TextValue text{*this, "txt"};

    /**
     * Background-color (bco) attribute of component
     */
    Color background{*this, "bco"};

    /**
     * Font-color (pco) attribute of component
     */
    Color fontColor{*this, "pco"};

    /**
     * Text alignment
     */
    Alignment alignment{*this};

    /**
     * Text font
     */
    Font font{*this};

    /**
     * Background image
     */
    BackgroundImage backgroundImage{*this};

    /**
     * Scroll-Dir attribute of component.
     */
    IntegerValue scrollDir{*this, "dir"};

    /**
     * Scroll-Distance attribute of component.
     */
    LimitedIntegerValue scrollDistance{*this, "dis", 2};

    /**
     * Cycle-Time attribute of component.
     */
    LimitedIntegerValue cycleTime{*this, "tim", 8};

    /**
     * Enable scrolling
     */
    bool enable(void) {
        NxtIo::sendCommand(std::string{getObjName()} + ".en=1");
        return NxtIo::recvRetCommandFinished();
    }

    /**
     * Disable scrolling
     */
    bool disable(void) {
        NxtIo::sendCommand(std::string{getObjName()} + ".en=0");
        return NxtIo::recvRetCommandFinished();
    }
};

/**
 * @}
 */

} // namespace Nxt