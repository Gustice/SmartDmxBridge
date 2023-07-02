#pragma once

#include "NexHardware.h"
#include "bases/Color.hpp"
#include "bases/NexTouch.h"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Hotspot component. 
 */
class Hotspot: public NexTouch
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Hotspot(uint8_t pid, uint8_t cid, std::string_view name) : NexTouch(pid, cid, name) {}
};
/**
 * @}
 */

}
