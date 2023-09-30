#pragma once

#include "../Nextion.hpp"
#include "Touch.hpp"

namespace nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * Text component.
 */
class Text : public Touch {
  public: /* methods */
    Text(Page &page, Port &port, uint8_t cid, std::string_view name) : Touch(page, port, cid, name) {}

    /**
     * Text attribute of component.
     */
    TextValue text{*this, "txt"};

    /**
     * Background-color (bco) attribute of component
     */
    Color background{*this, "bco"};

    /**
     * Font-color (pco) attribute of component
     */
    Color fontColor{*this, "pco"};

    /**
     * Text alignment
     */
    Alignment alignment{*this};

    /**
     * Text font
     */
    Font font{*this};

    /**
     * Background image
     */
    BackgroundImage backgroundImage{*this};
};

/**
 * @}
 */

} // namespace Nxt
