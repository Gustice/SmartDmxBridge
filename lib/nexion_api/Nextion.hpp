#pragma once

// Find Nextion DataSheets at: https://nextion.tech/datasheets/

#include "NxtPort.hpp"
// #include "Touch.hpp"
// #include "Button.hpp"
// #include "Crop.hpp"
// #include "Gauge.hpp"
// #include "Hotspot.hpp"
// #include "Page.hpp"
// #include "Picture.hpp"
// #include "ProgressBar.hpp"
// #include "Slider.hpp"
// #include "Text.hpp"
// #include "Waveform.hpp"
// #include "Timer.hpp"
// #include "Number.hpp"
// #include "DualStateButton.hpp"
// #include "Variable.hpp"
// #include "Checkbox.hpp"
// #include "Radio.hpp"
// #include "Scrolltext.hpp"
// #include "Gpio.hpp"
// #include "Rtc.hpp"

extern const std::string EndSequence;


/**
 * @addtogroup CoreAPI
 * @{
 */

class Nextion {
  public:
    // using SensingList = Nxt::Touch::SensingList;
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
    // void nexLoop(const SensingList &nex_listen_list);


  protected:
  private:
    void sendLog(nxt::LogSeverity level, std::string log);

    nxt::Port _port;
    nxt::LogCallback _logCallback;

    bool _isHealthy;
};

/**
 * @}
 */
