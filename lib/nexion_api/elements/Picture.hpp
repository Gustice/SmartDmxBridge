#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"
#include "bases/Elements.hpp"

namespace nxt {

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
    Picture(Page &page, Port &port, uint8_t cid, std::string_view name) : Touch(page, port, cid, name) {}
    
    /**
     * Picture attribute of component.
     */
    IntegerValue picture{*this, "pic"};
};

/**
 * @}
 */

}
