#pragma once

#include "Nextion.h"
#include "esp_log.h"
#include "uart.hpp"
#include "configModel.hpp"
#include "ValueCache.hpp"

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
    using setColorCallback = void(*)(AmbientColorSet);

    Display(Uart &port, std::string name, std::string version, ColorPresets & initialColors, setColorCallback colorSetCb) 
        : _port(port), _colorPresets(initialColors), _colorSetCb(colorSetCb) {
        if (!NxtIo::nexInit(_port, dumpLog)) {
            dumpLog(NxtLogSeverity::Error, "Init failed");
        }
        ESP_LOGI("DISP", "begin setup");
        tHealth.text.set("Setup Image");

        bScheme1.attachPush(bScheme1Cb, this);
        bScheme2.attachPush(bScheme2Cb, this);
        bScheme3.attachPush(bScheme3Cb, this);
        bSchemeCustom.attachPush(bSchemeCustomCb, this);
        hCustomFg.attachPop(hCustomFgCb, this);
        hCustomBg.attachPop(hCustomBgCb, this);

        bToInfoPage.attachPush(switchToInfoPage, this);
        bToWorkingPage.attachPush(switchToWorkingPage, this);
        
        switchToWorkingPage(this);
        ESP_LOGI("DISP", "setup finished");

        tName.text.set(name);
        tVersion.text.set(version);
        tAddress.text.set("0.0.0.0");
        tInfo.text.set("Device Info");
        tStatus.text.set("Device Status");

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

            jLight.value.set(0);
            jAmbient.value.set(0);
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

        if (_ip.isNew()) {
            tAddress.text.set(_ip.getValue());
        }
        if (_status.isNew()) {
            tStatus.text.set(_status.getValue());
        }
        if (_info.isNew()) {
            tInfo.text.set(_info.getValue());
        }
        
    }

    void setIp(std::string ip) {
        _ip.setValue(ip);
    }
    void setInfo(std::string info) {
        _info.setValue(info);
    }
    void setStatus(std::string status) {
        _status.setValue(status);
    }

    void setValues(std::array<uint8_t,2> intensities) {
        jLight.value.set(intensities[0]);
        jAmbient.value.set(intensities[1]);
    }

  private:
    static uint32_t calcColor (Color col) {
        return Nxt::Color::calcNextionColor(col.red, col.green, col.blue);
    }

    enum CurrentPage { WorkingPage = 1, InfoPage, Pages };

    UartWrapper _port;
    ColorPresets & _colorPresets;
    CurrentPage page = CurrentPage::WorkingPage;

    Nxt::Page splashScreen{0, "splashScreen"};
    Nxt::Page workingPage{1, "workingPage"};
    Nxt::Page infoPage{2, "infoPage"};

    Nxt::Button bScheme1{workingPage, 1, "bScheme1"};
    Nxt::Button bScheme2{workingPage, 4, "bScheme2"};
    Nxt::Button bScheme3{workingPage, 7, "bScheme3"};
    Nxt::Button bSchemeCustom{workingPage, 10, "bSchemeCustom"};

    Nxt::Button bToInfoPage{workingPage, 15, "bNext"};
    Nxt::Button bToWorkingPage{infoPage, 11, "bPrev"};

    Nxt::Text tHealth{splashScreen, 4, "tHealth"};
    Nxt::Text tScheme1Fg{workingPage, 2, "tScheme1Fg"};
    Nxt::Text tScheme1Bg{workingPage, 3, "tScheme1Bg"};
    Nxt::Text tScheme2Fg{workingPage, 5, "tScheme2Fg"};
    Nxt::Text tScheme2Bg{workingPage, 6, "tScheme2Bg"};
    Nxt::Text tScheme3Fg{workingPage, 8, "tScheme3Fg"};
    Nxt::Text tScheme3Bg{workingPage, 9, "tScheme3Bg"};
    Nxt::Text tCustomFg{workingPage, 16, "tCustomFg"};
    Nxt::Text tCustomBg{workingPage, 17, "tCustomBg"};

    Nxt::Slider hCustomFg{workingPage, 13, "hCustomFg"};
    Nxt::Slider hCustomBg{workingPage, 14, "hCustomBg"};
    Nxt::ProgressBar jLight{workingPage, 21, "jLight"};
    Nxt::ProgressBar jAmbient{workingPage, 20, "jAmbient"};

    Nxt::Text tName{infoPage, 3, "tName"};
    Nxt::Text tVersion{infoPage, 4, "tVersion"};
    Nxt::Text tAddress{infoPage, 6, "tAddress"};
    Nxt::Text tInfo{infoPage, 8, "tInfo"};
    Nxt::Text tStatus{infoPage, 10, "tStatus"};

    ValueCache<std::string> _ip;
    ValueCache<std::string> _status;
    ValueCache<std::string> _info;
    setColorCallback _colorSetCb;
    AmbientColorSet _customPreset;

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
    }

    static void bScheme1Cb(void *ptr)
    {
        Display * display = (Display*) ptr;
        display->_colorSetCb(display->_colorPresets.preset1);
    }
    static void bScheme2Cb(void *ptr)
    {
        Display * display = (Display*) ptr;
        display->_colorSetCb(display->_colorPresets.preset2);
    }
    static void bScheme3Cb(void *ptr)
    {
        Display * display = (Display*) ptr;
        display->_colorSetCb(display->_colorPresets.preset3);
    }
    static void bSchemeCustomCb(void *ptr)
    {
        Display * display = (Display*) ptr;
        display->_colorSetCb(display->_customPreset);
    }

    static void hCustomFgCb(void *ptr)
    {
        Display * display = (Display*) ptr;
        auto fg = display->hCustomFg.value.get();
        auto color = hueToRgb(fg);
        display->_customPreset.foregroundColor = color;
        display->tCustomFg.background.set(calcColor(color));
    }
    static void hCustomBgCb(void *ptr)
    {
        Display * display = (Display*) ptr;
        auto bg = display->hCustomBg.value.get();
        auto color = hueToRgb(bg);
        display->_customPreset.backgroundColor = color;
        display->tCustomBg.background.set(calcColor(color));
    }

    static void dumpLog(NxtLogSeverity level, std::string msg) {
        auto output = msg.c_str();
        switch (level)
        {
        case NxtLogSeverity::Debug:
            ESP_LOGD("DISP", "Log: %s", output);
            break;
        case NxtLogSeverity::Warning:
            ESP_LOGW("DISP", "Log: %s", output);
            break;
        case NxtLogSeverity::Error:
            ESP_LOGE("DISP", "Log: %s", output);
            break;
        }
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
