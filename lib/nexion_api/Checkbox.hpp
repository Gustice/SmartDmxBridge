#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Checkbox component. 
 *
 * Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref NexTouch::attachPop to satisfy your purpose. 
 * 
 * @warning Please do not call @ref NexTouch::attachPush on this component, even though you can. 
 */
class Checkbox: public NexTouch
{
public: /* methods */

    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string name);
     */
    Checkbox(uint8_t pid, uint8_t cid, std::string name) : NexTouch(pid, cid, name) {}
	
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
