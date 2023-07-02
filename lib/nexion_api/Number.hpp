#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * Number component.
 */
class Number : public NexTouch {
  public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Number(uint8_t pid, uint8_t cid, std::string_view name) : NexTouch(pid, cid, name) {}

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
     * Text alignment
     */
    Alignment alignment{*this};

    /**
     * Text font
     */
    Font font{*this};

    /**
     * Length attribute of component
     */
    IntegerValue length{*this, "lenth"};

    /**
     * Background image
     */
    BackgroundImage backgroundImage{*this};
};

/**
 * @}
 */

} // namespace Nxt