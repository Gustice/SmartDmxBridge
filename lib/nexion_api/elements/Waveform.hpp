#pragma once

#include "Nextion.hpp"
#include "bases/Object.hpp"

namespace nxt {

/**
 * @brief Waveform component.
 */
class Waveform : public Object {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Waveform(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Object(page, port, cid, name) {}

    /// @brief Add value to show.
    /// @param ch - channel of waveform(0-3).
    /// @param number - the value of waveform.
    /// return success flag
    bool addValue(uint8_t ch, uint8_t number) {
        if (ch > 3) {
            return false;
        }

        char buf[15] = {0};
        sprintf(buf, "add %u,%u,%u", getObjCid(), ch, number);

        _port.sendCommand(buf);
        return true;
    }

    ///@brief Background-color (bco) attribute of component
    Color backgroundColor{*this, "bco"};

    ///@brief GridColor (bco) attribute of component
    Color gridColor{*this, "gdc"};

    ///@brief Grid-width attribute of component.
    IntegerValue gridWidth{*this, "gdw"};

    ///@brief Grid-height attribute of component.
    IntegerValue gridHeight{*this, "gdh"};

    ///@brief Background-color (bco) attribute of component
    Color channelColor0{*this, "pco0"};
};

} // namespace nxt
