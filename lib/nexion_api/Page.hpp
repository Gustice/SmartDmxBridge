#pragma once

#include "Touch.hpp"
#include <map>

namespace Nxt {
/**
 * @addtogroup Component
 * @{
 */

/**
 * A special component , which can contain other components such as NexButton,
 * NexText and NexWaveform, etc.
 */
class Page : public Touch {
    friend class NxtIo;

  public: /* methods */
    /// @brief Page constructor
    /// @param pid page-id
    /// @param name page-name
    Page(uint8_t pid, std::string_view name) : Touch(name), _pageId(pid) {
        if (pid == 0) {
            _currentActive = this;
        }
        _pages[pid] = this;
    }

    /**
     * Show page.
     *
     * @return true if success, false for failure.
     */
    bool show(void);

    bool isActive() { return (_currentActive == this); }

    static const std::map<uint8_t, Page *> &getPageMap() {
        return _pages;
    }

    uint8_t getId() {
        return _pageId;
    };

  private:
    uint8_t _pageId;
    static Page *_currentActive;
    static std::map<uint8_t, Page *> _pages;
};

} // namespace Nxt