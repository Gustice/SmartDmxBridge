#pragma once

// Find Nextion DataSheets at: https://nextion.tech/datasheets/

#include "bases/Alignment.hpp"
#include "bases/BackgroundImage.hpp"
#include "bases/Color.hpp"
#include "bases/Font.hpp"
#include "bases/Port.hpp"
#include "elements/Page.hpp"
#include "Gpio.hpp"
#include "Rtc.hpp"

#include <map>

class Nextion {
  public:
    using SensingList = nxt::Touch::SensingList;
    /**
     * Init Nextion.
     *
     * @return true if success, false for failure.
     */
    Nextion(nxt::Stream &port, nxt::LogCallback logCb = nullptr);

    /**
     * Listen touch event and calling callbacks attached before.
     *
     * Supports push and pop at present.
     *
     * @param nex_listen_list - index to Nextion Components list.
     * @return none.
     *
     * @warning This function must be called repeatedly to response touch events
     *  from Nextion touch panel. Actually, you should place it in your loop function.
     */
    void nexLoop(const SensingList &nex_listen_list);

    nxt::Page createPage(uint8_t pageId, std::string_view name) {
        nxt::Page page(*this, _port, pageId, name);
        return page;
    }

    nxt::Rtc &getRtc() {
        return _timer;
    }

    nxt::Gpio &getGpio() {
        return _gpio;
    }

    nxt::Page &getActivePage() {
        return *_currentActive;
    }

    const std::map<uint8_t, nxt::Page *> &getPageMap() {
        return _pages;
    }

    bool showPage(nxt::Page & page) {
      _port.sendCommand(std::string("page ") + page.getObjName());
      bool isOk = _port.recvRetCommandFinished();
      if (isOk) {
          _currentActive = &page;
      }
      return isOk;
    }

  protected:
  private:
    void sendLog(nxt::LogSeverity level, std::string log);

    nxt::Port _port;
    nxt::LogCallback _logCallback;
    nxt::Rtc _timer;
    nxt::Gpio _gpio;

    bool _isHealthy;

    nxt::Page *_currentActive = nullptr;
    std::map<uint8_t, nxt::Page *> _pages;
};
