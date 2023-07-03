#pragma once

#include "NexHardware.h"
#include "Touch.hpp"
#include "bases/Elements.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Checkbox component. 
 *
 * Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref Touch::attachPop to satisfy your purpose. 
 * 
 * @warning Please do not call @ref Touch::attachPush on this component, even though you can. 
 */
class Checkbox: public Touch
{
public: /* methods */

    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Checkbox(Nxt::Page & pid, uint8_t cid, std::string_view name) : Touch(pid, cid, name) {}
	
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
};
/**
 * @}
 */

}
