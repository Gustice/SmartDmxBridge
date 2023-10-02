#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief ScrollText component.
 */
class Scrolltext : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Scrolltext(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}

    /// @brief Text attribute of component.
    TextValue text{*this, "txt"};

    /// @brief Background-color (bco) attribute of component
    Color background{*this, "bco"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};

    /// @brief Text alignment
    Alignment alignment{*this};

    /// @brief Text font
    Font font{*this};

    /// @brief Background image
    BackgroundImage backgroundImage{*this};

    /// @brief Scroll-Dir attribute of component.
    IntegerValue scrollDir{*this, "dir"};

    /// @brief Scroll-Distance attribute of component.
    LimitedIntegerValue scrollDistance{*this, "dis", 2};

    /// @brief Cycle-Time attribute of component.
    LimitedIntegerValue cycleTime{*this, "tim", 8};

    /// @brief Enable scrolling
    bool enable(void) {
        _port.sendCommand(std::string{getObjName()} + ".en=1");
        return _port.recvRetCommandFinished();
    }

    /// @brief Disable scrolling
    bool disable(void) {
        _port.sendCommand(std::string{getObjName()} + ".en=0");
        return _port.recvRetCommandFinished();
    }
};

} // namespace nxt