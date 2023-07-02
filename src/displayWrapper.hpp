#pragma once

#include "Nextion.h"
#include "esp_log.h"
#include "uart.hpp"

class UartWrapper : public SerialStream {
    public:
    UartWrapper(Uart & port) : _port(port) {}

    std::vector<uint8_t> read(size_t minSize = 0, unsigned msTimeout = 0) override {
        return _port.readBytes(minSize, msTimeout);
    }
    virtual void write(char c) override {
        _port.write(c);
    }
    virtual void write(std::string str) override {
        _port.write(str);
    }

    private:
    Uart &_port;
};

class Display {
  public:
    Display(Uart &port) : _port(port) {
        NxtIo::nexInit(_port, dumpLog);
        ESP_LOGI("DISP", "begin setup");
        tHealth.text.set("Setup Image");

        bScheme1.attachPop(bScheme1Cb, &bScheme1);
        bScheme2.attachPop(bScheme2Cb, &bScheme2);
        bScheme3.attachPop(bScheme3Cb, &bScheme3);
        bSchemeCustom.attachPop(bSchemeCustomCb, &bSchemeCustom);
        hCustomFg.attachPop(hCustomFgCb, &hCustomFg);
        hCustomBg.attachPop(hCustomBgCb, &hCustomBg);

        bToInfoPage.attachPop(switchToInfoPage, this);
        bToWorkingPage.attachPop(switchToWorkingPage, this);

        switchToWorkingPage(this);
        ESP_LOGI("DISP", "setup finished");

        page = CurrentPage::InfoPage;
        tName.text.set("DMX-Bridge");
        tVersion.text.set("T 0.0.0");
        tAddress.text.set("0.0.0.0");
        tInfo.text.set("Device Info");
        tStatus.text.set("Device Status");

        page = CurrentPage::WorkingPage;
        workingPage.show();
        tScheme1Fg.background.set(Nxt::Color::calcNextionColor(0xFF,0,0));
        tScheme2Fg.background.set(Nxt::Color::calcNextionColor(0,0xFF,0));
        tScheme3Fg.background.set(Nxt::Color::calcNextionColor(0,0,0xFF));
    }

    void tick() {
        switch (page) {
        case CurrentPage::WorkingPage: {
            NexTouch *nex_listen_list[] = {
                &bToInfoPage,   &bScheme1,  &bScheme2,  &bScheme3,
                &bSchemeCustom, &hCustomFg, &hCustomBg, nullptr,
            };
        NxtIo::nexLoop(nex_listen_list);

        } break;

        case CurrentPage::InfoPage: {
            NexTouch *nex_listen_list[] = {
                &bToWorkingPage,
                nullptr,
            };
        NxtIo::nexLoop(nex_listen_list);

        } break;

        default:
            break;
        }
    }

  private:
    enum CurrentPage { WorkingPage = 1, InfoPage, Pages };

    UartWrapper _port;
    CurrentPage page = CurrentPage::WorkingPage;

    NexPage workingPage{1, 0, "workingPage"};
    NexPage infoPage{2, 0, "infoPage"};

    Nxt::Button bScheme1{1, 1, "bScheme1"};
    Nxt::Button bScheme2{1, 4, "bScheme2"};
    Nxt::Button bScheme3{1, 7, "bScheme3"};
    Nxt::Button bSchemeCustom{1, 10, "bSchemeCustom"};

    Nxt::Button bToInfoPage{1, 15, "bNext"};
    Nxt::Button bToWorkingPage{2, 11, "bPrev"};

    Nxt::Text tHealth{0, 4, "tHealth"};
    Nxt::Text tScheme1Fg{1, 2, "tScheme1Fg"};
    Nxt::Text tScheme1Bg{1, 3, "tScheme1Bg"};
    Nxt::Text tScheme2Fg{1, 5, "tScheme2Fg"};
    Nxt::Text tScheme2Bg{1, 6, "tScheme2Bg"};
    Nxt::Text tScheme3Fg{1, 8, "tScheme3Fg"};
    Nxt::Text tScheme3Bg{1, 9, "tScheme3Bg"};
    Nxt::Text tCustomFg{1, 16, "tCustomFg"};
    Nxt::Text tCustomBg{1, 17, "tCustomBg"};

    Nxt::Slider hCustomFg{1, 1, "hCustomFg"};
    Nxt::Slider hCustomBg{1, 1, "hCustomBg"};

    Nxt::Text tName{2, 3, "tName"};
    Nxt::Text tVersion{2, 4, "tVersion"};
    Nxt::Text tAddress{2, 6, "tAddress"};
    Nxt::Text tInfo{2, 8, "tInfo"};
    Nxt::Text tStatus{2, 10, "tStatus"};

    static void switchToWorkingPage(void *ptr) {
        Display * display = (Display*) ptr;

    }
    static void switchToInfoPage(void *ptr) {
        Display * display = (Display*) ptr;
    }

    static void bScheme1Cb(void *ptr)
    {
        // todo ERROR
    }
    static void bScheme2Cb(void *ptr)
    {
        // todo ERROR
    }
    static void bScheme3Cb(void *ptr)
    {
        // todo ERROR
    }
    static void bSchemeCustomCb(void *ptr)
    {
        // todo ERROR
    }

    static void hCustomFgCb(void *ptr)
    {
        // Update Display color and color
    }
    static void hCustomBgCb(void *ptr)
    {
        // Update Display color and color
    }

    static void dumpLog(std::string msg) {
        auto output = msg.c_str();
        ESP_LOGD("DISP", "Log: %s", output);
    }
};