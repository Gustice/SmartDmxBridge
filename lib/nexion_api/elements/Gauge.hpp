#pragma once

#include "Nextion.hpp"
#include "bases/Object.hpp"

namespace nxt {

/**
 * @brief Gauge component.
 */
class Gauge : public Object {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Gauge(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Object(page, port, cid, name) {}

    /// @brief Text attribute of component.
    IntegerValue value{*this, "val"};

    /// @brief Background-color (bco) attribute of component
    Color background{*this, "bco"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};

    /// @brief Text attribute of component.
    IntegerValue pointerThickness{*this, "wid"};

    /// @brief Text attribute of component.
    IntegerValue backgroundCropImage{*this, "picc"};
};

} // namespace nxt
