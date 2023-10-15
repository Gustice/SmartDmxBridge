#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief Hotspot component. 
 */
class Hotspot : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Hotspot(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}
};

} // namespace nxt
