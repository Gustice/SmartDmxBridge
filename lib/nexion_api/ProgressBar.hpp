#pragma once

#include "NexHardware.h"
#include "bases/Object.hpp"
#include "bases/Elements.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * ProgressBar component. 
 */
class ProgressBar: public Object
{
public: /* methods */
    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    ProgressBar(uint8_t pid, uint8_t cid, std::string_view name) : Object(pid, cid, name) {}
    
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
