#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Crop component. 
 */
class Crop: public NexTouch
{
public: /* methods */

    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Crop(uint8_t pid, uint8_t cid, std::string_view name) : NexTouch(pid, cid, name) {}

    /**
     * Image attribute of component.
     */
    IntegerValue cropImage{*this, "picc"};
};

/**
 * @}
 */

}
