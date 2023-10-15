#pragma once

#include "Nextion.hpp"
#include "bases/Object.hpp"

namespace nxt {

/**
 * @brief ProgressBar component. 
 */
class ProgressBar : public Object {
  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    ProgressBar(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Object(page, port, cid, name) {}

    /// @brief Number attribute of component.
    IntegerValue value{*this, "val"};

    /// @brief Background-color (bco) attribute of component
    Color background{*this, "bco"};

    /// @brief Font-color (pco) attribute of component
    Color fontColor{*this, "pco"};
};

} // namespace nxt
