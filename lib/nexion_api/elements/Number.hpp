#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief Number component.
 */
class Number : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Number(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}

    /// @brief Number attribute of component.
    IntegerValue value{*this, "val"};

    /// @brief Background-color (bco) attribute of component
    Color background{*this, "bco"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};

    /// @brief Text alignment
    Alignment alignment{*this};

    /// @brief Text font
    Font font{*this};

    /// @brief Length attribute of component
    IntegerValue length{*this, "lenth"};

    /// @brief Background image
    BackgroundImage backgroundImage{*this};
};

} // namespace nxt