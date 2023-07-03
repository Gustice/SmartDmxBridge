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
 * Picture component. 
 */
class Picture: public Touch
{
public: /* methods */
    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Picture(Nxt::Page & pid, uint8_t cid, std::string_view name) : Touch(pid, cid, name) {}
    
    /**
     * Picture attribute of component.
     */
    IntegerValue picture{*this, "pic"};
};

/**
 * @}
 */

}
