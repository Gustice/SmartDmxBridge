#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief Checkbox component. 
 *
 * @details Commonly, you want to do something after push and pop it. It is recommended that only
 * call @ref Touch::attachPop to satisfy your purpose. 
 */
class Checkbox : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Checkbox(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}

    /// @brief Number attribute of component.
    IntegerValue value{*this, "val"};

    /// @brief Background-color (bco) attribute of component
    Color background{*this, "bco"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};
};

} // namespace nxt
