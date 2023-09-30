#pragma once

#include "Button.hpp"
#include "Crop.hpp"
#include "Gauge.hpp"
#include "Number.hpp"
#include "Scrolltext.hpp"
#include "Slider.hpp"
#include "Text.hpp"
#include "Touch.hpp"
#include "Picture.hpp"
#include "ProgressBar.hpp"
#include "Timer.hpp"
#include "Variable.hpp"
#include "Checkbox.hpp"
#include "Radio.hpp"
#include "DualStateButton.hpp"
#include "Hotspot.hpp"
#include "Waveform.hpp"

class Nextion;
namespace nxt {
/**
 * A special component , which can contain other components such as NexButton,
 * NexText and NexWaveform, etc.
 */
class Page : public Touch {

  public: /* methods */
    /// @brief Page constructor
    /// @param pid page-id
    /// @param name page-name
    Page(Nextion &parent, Port &port, uint8_t pid, std::string_view name)
        : Touch(name, port), _parent(parent), _port(port), _pageId(pid) {
    }

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
    bool show(void);

    bool isActive();

    uint8_t getId() {
        return _pageId;
    };

  private:
    Nextion &_parent;
    Port &_port;
    uint8_t _pageId;
};

} // namespace nxt