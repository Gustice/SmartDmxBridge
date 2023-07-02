#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * Waveform component.
 */
class Waveform : public NexObject {
  public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Waveform(uint8_t pid, uint8_t cid, std::string_view name) : NexObject(pid, cid, name) {}

    /**
     * Add value to show.
     *
     * @param ch - channel of waveform(0-3).
     * @param number - the value of waveform.
     *
     * @retval true - success.
     * @retval false - failed.
     */
    bool addValue(uint8_t ch, uint8_t number) {
        if (ch > 3) {
            return false;
        }

        char buf[15] = {0};
        sprintf(buf, "add %u,%u,%u", getObjCid(), ch, number);

        NxtIo::sendCommand(buf);
        return true;
    }

    /**
     * Background-color (bco) attribute of component
     */
    Color backgroundColor{*this, "bco"};

    /**
     * GridColor (bco) attribute of component
     */
    Color gridColor{*this, "gdc"};

    /**
     * Grid-width attribute of component.
     */
    IntegerValue gridWidth{*this, "gdw"};

    /**
     * Grid-height attribute of component.
     */
    IntegerValue gridHeight{*this, "gdh"};

    /**
     * Background-color (bco) attribute of component
     */
    Color channelColor0{*this, "pco0"};
};

/**
 * @}
 */

} // namespace Nxt
