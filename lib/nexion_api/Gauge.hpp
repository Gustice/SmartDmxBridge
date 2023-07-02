#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Gauge component.
 */
class Gauge: public NexObject
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Gauge(uint8_t pid, uint8_t cid, std::string_view name) : NexObject(pid, cid, name) {}

    /**
     * Text attribute of component.
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
     * Text attribute of component.
     */
    IntegerValue pointerThickness{*this, "wid"};

    /**
     * Text attribute of component.
     */
    IntegerValue backgroundCropImage{*this, "picc"};
};

/**
 * @}
 */

}
