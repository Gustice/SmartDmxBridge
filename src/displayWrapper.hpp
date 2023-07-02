#pragma once

#include "Nextion.h"
#include "esp_log.h"
#include "uart.hpp"
#include "configModel.hpp"

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
    Display(Uart &port, ColorPresets & initialColors) : _port(port), _colorPresets(initialColors) {
        NxtIo::nexInit(_port, dumpLog);
        ESP_LOGI("DISP", "begin setup");
        tHealth.text.set("Setup Image");

        bScheme1.attachPush(bScheme1Cb, &bScheme1);
        bScheme2.attachPush(bScheme2Cb, &bScheme2);
        bScheme3.attachPush(bScheme3Cb, &bScheme3);
        bSchemeCustom.attachPush(bSchemeCustomCb, &bSchemeCustom);
        hCustomFg.attachPop(hCustomFgCb, this);
        hCustomBg.attachPop(hCustomBgCb, this);

        bToInfoPage.attachPush(switchToInfoPage, this);
        bToWorkingPage.attachPush(switchToWorkingPage, this);

        switchToWorkingPage(this);
        ESP_LOGI("DISP", "setup finished");

        page = CurrentPage::WorkingPage;
        workingPage.show();
        {
            auto & ic = initialColors;
            tScheme1Fg.background.set(calcColor(ic.preset1.foregroundColor));
            tScheme1Bg.background.set(calcColor(ic.preset1.backgroundColor));
            tScheme2Fg.background.set(calcColor(ic.preset2.foregroundColor));
            tScheme2Bg.background.set(calcColor(ic.preset2.backgroundColor));
            tScheme3Fg.background.set(calcColor(ic.preset3.foregroundColor));
            tScheme3Bg.background.set(calcColor(ic.preset3.backgroundColor));

            auto fg = hCustomFg.value.get();
            tCustomFg.background.set(calcColor(hueToRgb(fg)));
            auto bg = hCustomBg.value.get();
            tCustomBg.background.set(calcColor(hueToRgb(bg)));
        }
    }

    void tick() {
        switch (page) {
        case CurrentPage::WorkingPage: {
            NxtIo::SensingList list = {
                &bToInfoPage,   &bScheme1,  &bScheme2, &bScheme3,
                &bSchemeCustom, &hCustomFg, &hCustomBg
            };
        NxtIo::nexLoop(list);

        } break;

        case CurrentPage::InfoPage: {
            NxtIo::SensingList list = {
                &bToWorkingPage
            };
        NxtIo::nexLoop(list);

        } break;

        default:
            break;
        }
    }

  private:
    static uint32_t calcColor (Color col) {
        return Nxt::Color::calcNextionColor(col.red, col.green, col.blue);
    }

    enum CurrentPage { WorkingPage = 1, InfoPage, Pages };

    UartWrapper _port;
    ColorPresets & _colorPresets;
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

    Nxt::Slider hCustomFg{1, 13, "hCustomFg"};
    Nxt::Slider hCustomBg{1, 14, "hCustomBg"};
    Nxt::ProgressBar jLight{2, 13, "jLight"};
    Nxt::ProgressBar jAmbient{2, 12, "jAmbient"};


    Nxt::Text tName{2, 3, "tName"};
    Nxt::Text tVersion{2, 4, "tVersion"};
    Nxt::Text tAddress{2, 6, "tAddress"};
    Nxt::Text tInfo{2, 8, "tInfo"};
    Nxt::Text tStatus{2, 10, "tStatus"};

    static void switchToWorkingPage(void *ptr) {
        Display * display = (Display*) ptr;
        display->page = CurrentPage::WorkingPage;
        //display->workingPage.show(); // already set by display
    }

    static void switchToInfoPage(void *ptr) {
        ESP_LOGW("DISP", "To Info-Page");
        Display * display = (Display*) ptr;
        display->page = CurrentPage::InfoPage;
        //display->infoPage.show(); // already set by display
        display->tName.text.set("DMX-Bridge");
        display->tVersion.text.set("T 0.0.0");
        display->tAddress.text.set("0.0.0.0");
        display->tInfo.text.set("Device Info");
        display->tStatus.text.set("Device Status");
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
        Display * display = (Display*) ptr;
        auto fg = display->hCustomFg.value.get();
        display->tCustomFg.background.set(calcColor(hueToRgb(fg)));
    }
    static void hCustomBgCb(void *ptr)
    {
        Display * display = (Display*) ptr;
        auto bg = display->hCustomBg.value.get();
        display->tCustomBg.background.set(calcColor(hueToRgb(bg)));
    }

    static void dumpLog(std::string msg) {
        auto output = msg.c_str();
        ESP_LOGD("DISP", "Log: %s", output);
    }

    static Color hueToRgb(uint8_t hue)
    {
        Color rgb;
        unsigned char region, remainder, q, t;
        region = hue / 43;
        remainder = (hue - (region * 43)) * 6; 
        q = (255 * (255 - ((255 * remainder) >> 8))) >> 8;
        t = (255 * (255 - ((255 * (255 - remainder)) >> 8))) >> 8;
        
        switch (region)
        {
            case 0:
                rgb.red = 255; rgb.green = t; rgb.blue = 0;
                break;
            case 1:
                rgb.red = q; rgb.green = 255; rgb.blue = 0;
                break;
            case 2:
                rgb.red = 0; rgb.green = 255; rgb.blue = t;
                break;
            case 3:
                rgb.red = 0; rgb.green = q; rgb.blue = 255;
                break;
            case 4:
                rgb.red = t; rgb.green = 0; rgb.blue = 255;
                break;
            default:
                rgb.red = 255; rgb.green = 0; rgb.blue = q;
                break;
        }
        
        return rgb;
    }
};















