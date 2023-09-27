#pragma once

#include "Nextion.h"
#include "configModel.hpp"
#include "uart.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace display {

/// @brief Wrapper class for UART port to mimic simple stream behaviour
class UartWrapper : public SerialStream {
  public:
    UartWrapper(Uart &port) : _port(port) {}

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

enum class Page { Splash = 0, Working, Info };

class PageBase {
  public:
    using EventCallback = void (*)(void *);
    using BArgs = void *;
    using BindCb = std::function<void(BArgs)>;
    using SwitchCb = std::function<void(display::PageBase *)>;

    PageBase(Page id, std::string_view name, EventCallback genericCb)
        : _page(static_cast<uint8_t>(id), name), _genericCb(genericCb) {}
    virtual void init(){};
    virtual void tick(){};

    static void *createFunctional(BindCb cb) {
        BindCb *c = new BindCb{cb};
        return (void *)c;
    }

    PageBase *show() {
        _page.show();
        return this;
    }

    std::string getName() {
        return _page.getObjName();
    }

  protected:
    Nxt::Page _page;
    EventCallback _genericCb;
};

class SplashScreenPage : public PageBase {
  public:
    SplashScreenPage(EventCallback genericCb) : PageBase(Page::Splash, "splashScreen", genericCb) {}

    Nxt::Text tHealth{_page, 4, "tHealth"};

    void init() override {
        tHealth.text.set("Setup Image");
    }
};

class WorkingPage : public PageBase {
  public:
    using setColorCallback = void (*)(AmbientColorSet);

    WorkingPage(EventCallback genericCb, SwitchCb pageSwitch, ColorPresets &presets,
                setColorCallback colorCb)
        : PageBase(Page::Working, "workingPage", genericCb), _colorSetCb(colorCb),
          _colorPresets(presets), _pageSwitchCb(pageSwitch) {
        auto gCb = _genericCb;
        auto cF = createFunctional;

        bScheme1.attachPush(gCb, cF([this](BArgs a) { _colorSetCb(_colorPresets.preset1); }));
        bScheme2.attachPush(gCb, cF([this](BArgs a) { this->bScheme2Cb(a); }));
        bScheme3.attachPush(gCb, cF([this](BArgs a) { this->bScheme3Cb(a); }));
        bSchemeCustom.attachPush(gCb, cF([this](BArgs a) { this->bSchemeCustomCb(a); }));
        hCustomFg.attachPop(gCb, cF([this](BArgs a) { this->hCustomFgCb(a); }));
        hCustomBg.attachPop(gCb, cF([this](BArgs a) { this->hCustomBgCb(a); }));
        bToInfoPage.attachPush(gCb, cF([this](BArgs a) { this->switchPage(a); }));
    }

    Nxt::Button bScheme1{_page, 1, "bScheme1"};
    Nxt::Button bScheme2{_page, 4, "bScheme2"};
    Nxt::Button bScheme3{_page, 7, "bScheme3"};
    Nxt::Button bSchemeCustom{_page, 10, "bSchemeCustom"};

    Nxt::Button bToInfoPage{_page, 15, "bNext"};

    Nxt::Text tScheme1Fg{_page, 2, "tScheme1Fg"};
    Nxt::Text tScheme1Bg{_page, 3, "tScheme1Bg"};
    Nxt::Text tScheme2Fg{_page, 5, "tScheme2Fg"};
    Nxt::Text tScheme2Bg{_page, 6, "tScheme2Bg"};
    Nxt::Text tScheme3Fg{_page, 8, "tScheme3Fg"};
    Nxt::Text tScheme3Bg{_page, 9, "tScheme3Bg"};
    Nxt::Text tCustomFg{_page, 16, "tCustomFg"};
    Nxt::Text tCustomBg{_page, 17, "tCustomBg"};

    Nxt::Slider hCustomFg{_page, 13, "hCustomFg"};
    Nxt::Slider hCustomBg{_page, 14, "hCustomBg"};
    Nxt::ProgressBar jLight{_page, 21, "jLight"};
    Nxt::ProgressBar jAmbient{_page, 20, "jAmbient"};

    void init() override {
        auto &ic = _colorPresets;
        tScheme1Fg.background.set(calcColor(ic.preset1.foregroundColor));
        tScheme1Bg.background.set(calcColor(ic.preset1.backgroundColor));
        tScheme2Fg.background.set(calcColor(ic.preset2.foregroundColor));
        tScheme2Bg.background.set(calcColor(ic.preset2.backgroundColor));
        tScheme3Fg.background.set(calcColor(ic.preset3.foregroundColor));
        tScheme3Bg.background.set(calcColor(ic.preset3.backgroundColor));

        auto fg = hCustomFg.value.get();
        tCustomFg.background.set(calcColor(Color::fromHue(fg)));
        auto bg = hCustomBg.value.get();
        tCustomBg.background.set(calcColor(Color::fromHue(bg)));

        jLight.value.set(0);
        jAmbient.value.set(0);
    }

    void tick() override {
        NxtIo::nexLoop(_sensingList);
    }

    void bScheme2Cb(void *ptr) {
        _colorSetCb(_colorPresets.preset2);
    }
    void bScheme3Cb(void *ptr) {
        _colorSetCb(_colorPresets.preset3);
    }
    void bSchemeCustomCb(void *ptr) {
        _colorSetCb(_customPreset);
    }

    void hCustomFgCb(void *ptr) {
        auto fg = hCustomFg.value.get();
        auto color = Color::fromHue(fg);
        _customPreset.foregroundColor = color;
        tCustomFg.background.set(calcColor(color));
    }
    void hCustomBgCb(void *ptr) {
        auto bg = hCustomBg.value.get();
        auto color = Color::fromHue(bg);
        _customPreset.backgroundColor = color;
        tCustomBg.background.set(calcColor(color));
    }

    void switchPage(void *ptr) {
        _pageSwitchCb(this);
    }

    static uint32_t calcColor(Color col) {
        return Nxt::Color::calcNextionColor(col.red, col.green, col.blue);
    }

    setColorCallback _colorSetCb;
    ColorPresets &_colorPresets;
    SwitchCb _pageSwitchCb;

    AmbientColorSet _customPreset;
    NxtIo::SensingList _sensingList = {&bToInfoPage,   &bScheme1,  &bScheme2, &bScheme3,
                                       &bSchemeCustom, &hCustomFg, &hCustomBg};
};

class InfoPage : public PageBase {
  public:
    InfoPage(EventCallback genericCb, SwitchCb pageSwitch, const std::string &name,
             const std::string &version)
        : PageBase(Page::Info, "infoPage", genericCb), _name(name), _version(version),
          _pageSwitchCb(pageSwitch) {
        bToWorkingPage.attachPush(_genericCb,
                                  createFunctional([this](BArgs a) { this->switchPage(a); }));
    }

    Nxt::Button bToWorkingPage{_page, 11, "bPrev"};

    Nxt::Text tName{_page, 3, "tName"};
    Nxt::Text tVersion{_page, 4, "tVersion"};
    Nxt::Text tAddress{_page, 6, "tAddress"};
    Nxt::Text tInfo{_page, 8, "tInfo"};
    Nxt::Text tStatus{_page, 10, "tStatus"};

    void init() override {
        tName.text.set(_name);
        tVersion.text.set(_version);
        tAddress.text.set("0.0.0.0");
        tInfo.text.set("Device Info");
        tStatus.text.set("Device Status");
    }

    void switchPage(void *ptr) {
        _pageSwitchCb(this);
    }

    void tick() override {
        NxtIo::nexLoop(_sensingList);
    }

  private:
    NxtIo::SensingList _sensingList = {&bToWorkingPage};
    const std::string &_name;
    const std::string &_version;
    SwitchCb _pageSwitchCb;
};

} // namespace display
