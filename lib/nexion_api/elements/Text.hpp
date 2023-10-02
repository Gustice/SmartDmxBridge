#pragma once

#include "../Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief Text component.
 */
class Text : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Text(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}

    /// @brief Text attribute of component.
    TextValue text{*this, "txt"};

    /// @brief Background-color (bco) attribute of component
    Color background{*this, "bco"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};

    /// @brief Text alignment
    Alignment alignment{*this};

    /// @brief Text font
    Font font{*this};

    /// @brief Background image
    BackgroundImage backgroundImage{*this};
};

} // namespace nxt
