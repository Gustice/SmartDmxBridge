#pragma once

#include "Button.hpp"
#include "Checkbox.hpp"
#include "Crop.hpp"
#include "DualStateButton.hpp"
#include "Gauge.hpp"
#include "Hotspot.hpp"
#include "Number.hpp"
#include "Picture.hpp"
#include "ProgressBar.hpp"
#include "Radio.hpp"
#include "Scrolltext.hpp"
#include "Slider.hpp"
#include "Text.hpp"
#include "Timer.hpp"
#include "Touch.hpp"
#include "Variable.hpp"
#include "Waveform.hpp"

class Nextion;
namespace nxt {
/**
 * @brief Page component
 * 
 * @details A special component , which can contain other components such as Button,
 * Text and Waveform, etc.
 */
class Page : public Touch {

  public:
    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Page(Nextion &parent, Port &port, uint8_t pid, std::string_view name)
        : Touch(name, port), _parent(parent), _port(port), _pageId(pid) {
    }

    /// @brief Component factory
    /// @tparam T Type of component like Text, Button, ...
    /// @param elementId Id on given page for referencing
    /// @param name name on given page for referencing
    /// @return Component instance
    template <typename T>
    T createComponent(uint8_t elementId, std::string_view name) {
        T element(*this, _port, elementId, name);
        return element;
    }

    /**
     * Show page.
     *
     * @return true if success, false for failure.
     */

    /// @brief Show page command
    /// @return success flag
    bool show();

    /// @brief Is active query
    /// @return true if page is active
    bool isActive();

    /// @brief Getter for PageId
    /// @return PageId
    uint8_t getId() {
        return _pageId;
    };

  private:
    Nextion &_parent;
    Port &_port;
    uint8_t _pageId;
};

} // namespace nxt