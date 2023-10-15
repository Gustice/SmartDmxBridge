#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @brief Variable component. 
 *
 * @details Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref Touch::attachPop to satisfy your purpose. 
 */
class Variable : public Touch {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Variable(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Touch(page, port, cid, name) {}

    /// @brief Text attribute of component.
    TextValue text{*this, "txt"};

    /// @brief Number attribute of component.
    IntegerValue value{*this, "val"};
};

} // namespace nxt
