#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Picture component. 
 */
class Picture: public NexTouch
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string name);
     */
    Picture(uint8_t pid, uint8_t cid, std::string name) : NexTouch(pid, cid, name) {}
    
    /**
     * Picture attribute of component.
     */
    IntegerValue picture{*this, "pic"};
};

/**
 * @}
 */

}
