#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * ProgressBar component. 
 */
class ProgressBar: public NexObject
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string name);
     */
    ProgressBar(uint8_t pid, uint8_t cid, std::string name) : NexObject(pid, cid, name) {}
    
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
