/**
 * @file displayWrapper.hpp
 * @author Gustice
 * @brief Wrapping Display control
 * @date 2023-09-24
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "ValueCache.hpp"
#include "configModel.hpp"
#include "displayElements.hpp"
#include "message_queue.hpp"
#include <esp_log.h>

/**
 * @brief Class for Device-Display
 */
class Display {
  public:
    /// @brief Propagate type for color callback
    using setColorCallback = display::WorkingPage::setColorCallback;
    /// @brief Display message object for message events
    struct Message {
        enum Type {
            UpdatedIp,    ///< Update IP-text
            UpdateInfo,   ///< Update Info-textfield
            UpdateStatus, ///< Update Status-textfield
        };
        /// @brief Message type
        Type type;
        /// @brief Message payload
        std::string message;

        /// @brief Constructor
        /// @param t message-type
        /// @param msg payload
        Message(Type t, std::string msg)
            : type(t), message(msg) {}
    };

    /// @brief Constructor
    /// @param port Reference to serial port
    /// @param name Device name for display
    /// @param version Device version for display
    /// @param presets Reference to color presets
    /// @param colorCb Callback to color switch
    Display(Uart &port, const std::string &name, const std::string &version, ColorPresets &presets,
            setColorCallback colorCb)
        : _port(port), pageSwitchFunc([this](display::PageBase *p) { this->pageSwitch(p); }),
          _display(_port, dumpLog), splashScreen(_display, eventCallback), workingPage(_display, eventCallback, presets, pageSwitchFunc, colorCb),
          infoPage(_display, eventCallback, name, version, pageSwitchFunc) {
        ESP_LOGI("DISP", "begin setup");

        splashScreen.init();
        infoPage.init();

        _page = workingPage.show();
        workingPage.init();
    }

    /// @brief Cyclic task
    void tick() {
        _page->tick();
    }

    /// @brief Process message for info-page
    /// @param queue queued message
    void processQueue(MessageQueue<Message> &queue) {
        std::unique_ptr<Message> pMsg;
        pMsg = queue.dequeue(0); // don't block if no message pending
        if (pMsg == nullptr) {
            return;
        }

        switch (pMsg->type) {
        case Message::Type::UpdatedIp:
            infoPage.tAddress.text.set(pMsg->message);
            break;

        case Message::Type::UpdateInfo:
            infoPage.tInfo.text.set(pMsg->message);
            break;
        case Message::Type::UpdateStatus:
            infoPage.tStatus.text.set(pMsg->message);
            break;
        }
    }

    /// @brief Set values for visualisation
    /// @param intensities
    void setValues(std::array<uint8_t, 2> intensities) {
        workingPage.jLight.value.set(intensities[0]);
        workingPage.jAmbient.value.set(intensities[1]);
    }

    /// @brief Callback for switch page
    /// @param to target page
    void pageSwitch(display::PageBase *to) {
        _page = to;
        ESP_LOGI("DISP", "Switch to %s", _page->getName().c_str());
        // display->infoPage.show(); // already set by display
    }

  private:
    display::UartWrapper _port;
    display::PageBase::SwitchCb pageSwitchFunc;

    Nextion _display;
    display::SplashScreenPage splashScreen;
    display::WorkingPage workingPage;
    display::InfoPage infoPage;

    display::PageBase *_page;

    static void dumpLog(nxt::LogSeverity level, std::string msg) {
        auto output = msg.c_str();
        switch (level) {
        case nxt::LogSeverity::Debug:
            ESP_LOGD("DISP", "Log: %s", output);
            break;
        case nxt::LogSeverity::Warning:
            ESP_LOGW("DISP", "Log: %s", output);
            break;
        case nxt::LogSeverity::Error:
            ESP_LOGE("DISP", "Log: %s", output);
            break;
        }
    }

    /// @brief Static method to generate c-compatible function-pointer for bound-command-callback
    /// @details - receives and wraps arguments in string,
    ///          - determines object according to appContext
    ///          - calls appropriate method of appropriate object
    static void eventCallback(void *context) {
        if (context == nullptr) {
            return;
            ESP_LOGE("DISP", "No context in callback available");
        }

        display::PageBase::BindCb &cb = *((display::PageBase::BindCb *)context);
        cb(nullptr);
    }
};
