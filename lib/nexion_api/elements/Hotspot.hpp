#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

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
    Hotspot(Page &page, Port &port, uint8_t cid, std::string_view name) : Touch(page, port, cid, name) {}
};
/**
 * @}
 */

}
