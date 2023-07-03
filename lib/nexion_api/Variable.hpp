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
 * Variable component. 
 *
 * Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref Touch::attachPop to satisfy your purpose. 
 * 
 * @warning Please do not call @ref Touch::attachPush on this component, even though you can. 
 */
class Variable: public Touch
{
public: /* methods */

    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Variable(Nxt::Page & pid, uint8_t cid, std::string_view name)  : Touch(pid, cid, name) {}

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
