#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {
/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Variable component. 
 *
 * Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref NexTouch::attachPop to satisfy your purpose. 
 * 
 * @warning Please do not call @ref NexTouch::attachPush on this component, even though you can. 
 */
class Variable: public NexTouch
{
public: /* methods */

    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string name);
     */
    Variable(uint8_t pid, uint8_t cid, std::string name)  : NexTouch(pid, cid, name) {}

    /**
     * Text attribute of component.
     */
    TextValue text{*this, "txt"};
	
    /**
     * Number attribute of component.
     */
    IntegerValue value{*this, "val"};
};
/**
 * @}
 */

}
