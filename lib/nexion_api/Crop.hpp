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
 * Crop component. 
 */
class Crop: public Touch
{
public: /* methods */

    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Crop(uint8_t pid, uint8_t cid, std::string_view name) : Touch(pid, cid, name) {}

    /**
     * Image attribute of component.
     */
    IntegerValue cropImage{*this, "picc"};
};

/**
 * @}
 */

}
