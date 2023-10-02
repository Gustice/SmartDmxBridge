#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief DualStateButton component. 
 *
 * @details Commonly, you want to do something after push and pop it. It is recommended that only
 * call @ref Touch::attachPop to satisfy your purpose. 
 */
class DualStateButton : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);    DualStateButton(Page &page, Port &port, uint8_t cid, std::string_view name) : Touch(page, port, cid, name) {}

    /// @brief Number attribute of component.
    IntegerValue value{*this, "val"};

    /// @brief Text attribute of component.
    TextValue text{*this, "txt"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};

    /// @brief Color State 1 attribute of component
    Color colorState0{*this, "bco0"};

    /// @brief Color State 2 attribute of component
    Color colorState1{*this, "bco1"};

    /// @brief Text alignment
    Alignment alignment{*this};

    /// @brief Text font
    Font font{*this};

    /// @brief Background image state 1
    BackgroundImage imageState1{*this, "0"};

    /// @brief Background image state 2
    BackgroundImage imageState2{*this, "1"};
};

} // namespace nxt
