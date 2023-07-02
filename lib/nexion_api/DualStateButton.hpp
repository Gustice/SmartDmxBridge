#pragma once

#include "NexHardware.h"
#include "bases/Touch.hpp"
#include "bases/Elements.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * DSButton component. 
 *
 * Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref Touch::attachPop to satisfy your purpose. 
 * 
 * @warning Please do not call @ref Touch::attachPush on this component, even though you can. 
 */
class DSButton: public Touch
{
public: /* methods */
    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    DSButton(uint8_t pid, uint8_t cid, std::string_view name) : Touch(pid, cid, name) {}
    
    /**
     * Number attribute of component.
     */
    IntegerValue value{*this, "val"};

    /**
     * Text attribute of component.
     */
    TextValue text{*this, "txt"};

    /**
     * Font-color (pco) attribute of component
     */
    Color fontColor{*this, "pco"};

    /**
     * Color State 1 attribute of component
     */
    Color colorState0{*this, "bco0"};

    /**
     * Color State 2 attribute of component
     */
    Color colorState1{*this, "bco1"};

    /**
     * Text alignment
     */
    Alignment alignment{*this};

    /**
     * Text font
     */
    Font font{*this};

    /**
     * Background image state 1
     */
    BackgroundImage imageState1{*this, "0"};

    /**
     * Background image state 2
     */
    BackgroundImage imageState2{*this, "1"};
};
/**
 * @}
 */

}
