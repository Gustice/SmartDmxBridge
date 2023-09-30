#pragma once

#include "Nextion.hpp"
#include "bases/Object.hpp"
#include "bases/Elements.hpp"

namespace nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Gauge component.
 */
class Gauge: public Object
{
public: /* methods */
    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Gauge(Page &page, Port &port, uint8_t cid, std::string_view name) : Object(page, port, cid, name) {}

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
