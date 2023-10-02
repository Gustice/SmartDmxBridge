#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief Button component.
 *
 * @details Commonly, you want to do something after push and pop it. It is recommended that only
 * call @ref Touch::attachPop to satisfy your purpose.
 */
class Button : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Button(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}

    /// @brief Text attribute of component.
    TextValue text{*this, "txt"};

    /// @brief Background-color (bco) attribute of component
    Color background{*this, "bco"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};

    /// @brief Background-color (bco) attribute of component in active state
    Color backgroundPressed{*this, "bco2"};

    /// @brief Font-color (pco) attribute of component in active state
    Color fontColorPressed{*this, "pco2"};

    /// @brief Text alignment
    Alignment alignment{*this};

    /// @brief Text font
    Font font{*this};

    /// @brief Background image
    BackgroundImage backgroundImage{*this};

    /// @brief Background image
    BackgroundImage backgroundImagePressed{*this, "2"};
};

} // namespace nxt