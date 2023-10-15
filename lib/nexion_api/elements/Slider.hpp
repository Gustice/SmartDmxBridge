#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"
#include "bases/Color.hpp"

namespace nxt {

/**
 * @brief Slider component.
 */
class Slider : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Slider(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}

    /// @brief Number attribute of component.
    IntegerValue value{*this, "val"};

    /// @brief Background-color (bco) attribute of component
    Color background{*this, "bco"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};

    /// @brief Pointer thickness of component.
    IntegerValue pointerThickness{*this, "wid"};

    /// @brief Cursor height of component.
    IntegerValue cursorHeight{*this, "hig"};

    /// @brief Maximum of component.
    IntegerValue maxValue{*this, "maxval"};

    /// @brief Minimum of component.
    IntegerValue minValue{*this, "minval"};
};

} // namespace nxt