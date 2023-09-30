#pragma once

#include "Nextion.hpp"
#include "configModel.hpp"
#include "uart.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace display {

/// @brief Wrapper class for UART port to mimic simple stream behaviour
class UartWrapper : public nxt::Stream {
  public:
    UartWrapper(Uart &port)
        : _port(port) {}

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

enum class Page { Splash = 0,
                  Working,
                  Info };

class PageBase {
  public:
    using EventCallback = void (*)(void *);
    using BArgs = void *;
    using BindCb = std::function<void(BArgs)>;
    using SwitchCb = std::function<void(display::PageBase *)>;

    PageBase(Nextion &display, Page id, std::string_view name, EventCallback genericCb)
        : _display(display),
          _page(display.createPage(static_cast<uint8_t>(id), name)),
          _genericCb(genericCb) {}
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
    Nextion &_display;
    nxt::Page _page;
    EventCallback _genericCb;
};

class SplashScreenPage : public PageBase {
  public:
    SplashScreenPage(Nextion &display, EventCallback genericCb)
        : PageBase(display, Page::Splash, "splashScreen", genericCb),
          tHealth(_page.createComponent<nxt::Text>(4, "tHealth")) {}

    nxt::Text tHealth;

    void init() override {
        tHealth.text.set("Setup Image");
    }
};

class WorkingPage : public PageBase {
  public:
    using setColorCallback = void (*)(AmbientColorSet);

    WorkingPage(Nextion &display, EventCallback genericCb, SwitchCb pageSwitch, ColorPresets &presets,
                setColorCallback colorCb)
        : PageBase(display, Page::Working, "workingPage", genericCb),
          bScheme1(_page.createComponent<nxt::Button>(1, "bScheme1")),
          bScheme2(_page.createComponent<nxt::Button>(4, "bScheme2")),
          bScheme3(_page.createComponent<nxt::Button>(7, "bScheme3")),
          bSchemeCustom(_page.createComponent<nxt::Button>(10, "bSchemeCustom")),
          bToInfoPage(_page.createComponent<nxt::Button>(15, "bNext")),
          tScheme1Fg(_page.createComponent<nxt::Text>(2, "tScheme1Fg")),
          tScheme1Bg(_page.createComponent<nxt::Text>(3, "tScheme1Bg")),
          tScheme2Fg(_page.createComponent<nxt::Text>(5, "tScheme2Fg")),
          tScheme2Bg(_page.createComponent<nxt::Text>(6, "tScheme2Bg")),
          tScheme3Fg(_page.createComponent<nxt::Text>(8, "tScheme3Fg")),
          tScheme3Bg(_page.createComponent<nxt::Text>(9, "tScheme3Bg")),
          tCustomFg(_page.createComponent<nxt::Text>(16, "tCustomFg")),
          tCustomBg(_page.createComponent<nxt::Text>(17, "tCustomBg")),
          hCustomFg(_page.createComponent<nxt::Slider>(13, "hCustomFg")),
          hCustomBg(_page.createComponent<nxt::Slider>(14, "hCustomBg")),
          jLight(_page.createComponent<nxt::ProgressBar>(21, "jLight")),
          jAmbient(_page.createComponent<nxt::ProgressBar>(20, "jAmbient")),
          _colorSetCb(colorCb),
          _colorPresets(presets), _pageSwitchCb(pageSwitch)
    {
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

    nxt::Button bScheme1;
    nxt::Button bScheme2;
    nxt::Button bScheme3;
    nxt::Button bSchemeCustom;
    nxt::Button bToInfoPage;
    nxt::Text tScheme1Fg;
    nxt::Text tScheme1Bg;
    nxt::Text tScheme2Fg;
    nxt::Text tScheme2Bg;
    nxt::Text tScheme3Fg;
    nxt::Text tScheme3Bg;
    nxt::Text tCustomFg;
    nxt::Text tCustomBg;
    nxt::Slider hCustomFg;
    nxt::Slider hCustomBg;
    nxt::ProgressBar jLight;
    nxt::ProgressBar jAmbient;

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
        _display.nexLoop(_sensingList);
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
        return nxt::Color::calcColor(col.red, col.green, col.blue);
    }

    setColorCallback _colorSetCb;
    ColorPresets &_colorPresets;
    SwitchCb _pageSwitchCb;

    AmbientColorSet _customPreset;
    Nextion::SensingList _sensingList = {&bToInfoPage, &bScheme1, &bScheme2, &bScheme3,
                                         &bSchemeCustom, &hCustomFg, &hCustomBg};
};

class InfoPage : public PageBase {
  public:
    InfoPage(Nextion &display, EventCallback genericCb, SwitchCb pageSwitch, const std::string &name,
             const std::string &version)
        : PageBase(display, Page::Info, "infoPage", genericCb),
          bToWorkingPage(_page.createComponent<nxt::Button>(11, "bPrev")),
          tName(_page.createComponent<nxt::Text>(3, "tName")),
          tVersion(_page.createComponent<nxt::Text>(4, "tVersion")),
          tAddress(_page.createComponent<nxt::Text>(6, "tAddress")),
          tInfo(_page.createComponent<nxt::Text>(8, "tInfo")),
          tStatus(_page.createComponent<nxt::Text>(10, "tStatus")),
          _name(name), _version(version), _pageSwitchCb(pageSwitch) {
        bToWorkingPage.attachPush(_genericCb,
                                  createFunctional([this](BArgs a) { this->switchPage(a); }));
    }

    nxt::Button bToWorkingPage;
    nxt::Text tName;
    nxt::Text tVersion;
    nxt::Text tAddress;
    nxt::Text tInfo;
    nxt::Text tStatus;

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
        _display.nexLoop(_sensingList);
    }

  private:
    Nextion::SensingList _sensingList = {&bToWorkingPage};
    const std::string &_name;
    const std::string &_version;
    SwitchCb _pageSwitchCb;
};

} // namespace display
