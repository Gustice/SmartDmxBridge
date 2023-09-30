#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"
#include "bases/Elements.hpp"

namespace nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * ScrollText component.
 */
class Scrolltext : public Touch {
  public: /* methods */
    Scrolltext(Page &page, Port &port, uint8_t cid, std::string_view name) : Touch(page, port, cid, name) {}

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
        _port.sendCommand(std::string{getObjName()} + ".en=1");
        return _port.recvRetCommandFinished();
    }

    /**
     * Disable scrolling
     */
    bool disable(void) {
        _port.sendCommand(std::string{getObjName()} + ".en=0");
        return _port.recvRetCommandFinished();
    }
};

/**
 * @}
 */

} // namespace Nxt