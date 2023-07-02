#pragma once

#include "NexHardware.h"
#include "bases/Color.hpp"
#include "bases/NexTouch.h"

namespace Nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * Slider component.
 */
class Slider : public NexTouch {
  public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Slider(uint8_t pid, uint8_t cid, std::string_view name) : NexTouch(pid, cid, name) {}

    /**
     * Number attribute of component.
     */
    IntegerValue value{*this, "val"};

    /**
     * Background-color (bco) attribute of component
     */
    Color background{*this, "bco"};

    /**
     * Font-color (pco) attribute of component
     */
    Color fontColor{*this, "pco"};

    /**
     * Pointer thickness of component.
     */
    IntegerValue pointerThickness{*this, "wid"};

    /**
     * Cursor height of component.
     */
    IntegerValue cursorHeight{*this, "hig"};

    /**
     * Maximum of component.
     */
    IntegerValue maxValue{*this, "maxval"};

    /**
     * Minimum of component.
     */
    IntegerValue minValue{*this, "minval"};
};
/**
 * @}
 */

} // namespace Nxt