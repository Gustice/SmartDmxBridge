#pragma once

#include "NexHardware.h"
#include "bases/Touch.hpp"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Hotspot component. 
 */
class Hotspot: public Touch
{
public: /* methods */
    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Hotspot(uint8_t pid, uint8_t cid, std::string_view name) : Touch(pid, cid, name) {}
};
/**
 * @}
 */

}
