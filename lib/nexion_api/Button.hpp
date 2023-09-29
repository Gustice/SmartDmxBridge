#pragma once

#include "Nextion.hpp"
#include "Touch.hpp"
#include "bases/Elements.hpp"

namespace Nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * NexButton component.
 *
 * Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref Touch::attachPop to satisfy your purpose.
 *
 * @warning Please do not call @ref Touch::attachPush on this component, even though you can.
 */
class Button : public Touch {
  public: /* methods */
    /**
     * @copydoc Object::Object(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Button(Nxt::Page & pid, uint8_t cid, std::string_view name) : Touch(pid, cid, name) {}

    /**
     * Text attribute of component.
     */
    Nxt::TextValue text{*this, "txt"};

    /**
     * Background-color (bco) attribute of component
     */
    Color background{*this, "bco"};

    /**
     * Font-color (pco) attribute of component
     */
    Color fontColor{*this, "pco"};

    /**
     * Background-color (bco) attribute of component in active state
     */
    Color backgroundPressed{*this, "bco2"};

    /**
     * Font-color (pco) attribute of component in active state
     */
    Color fontColorPressed{*this, "pco2"};

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

    /**
     * Background image
     */
    BackgroundImage backgroundImagePressed{*this, "2"};
};
/**
 * @}
 */

} // namespace Nxt