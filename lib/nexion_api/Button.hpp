#pragma once

#include "NexHardware.h"
#include "bases/BaseIncludes.hpp"

namespace Nxt {

/**
 * @addtogroup Component
 * @{
 */

/**
 * NexButton component.
 *
 * Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref NexTouch::attachPop to satisfy your purpose.
 *
 * @warning Please do not call @ref NexTouch::attachPush on this component, even though you can.
 */
class Button : public NexTouch {
  public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    Button(uint8_t pid, uint8_t cid, std::string_view name) : NexTouch(pid, cid, name) {}

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