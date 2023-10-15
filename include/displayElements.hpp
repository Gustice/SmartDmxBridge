/**
 * @file displayElements.hpp
 * @author Gustice
 * @brief Base classes and elements for Display control
 * @date 2023-10-02
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "Nextion.hpp"
#include "configModel.hpp"
#include "uart.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace display {

/**
 * @brief Wrapper class for UART port to mimic simple stream behaviour
 */
class UartWrapper : public nxt::Stream {
  public:
    /// @brief Constructor
    /// @param port reference to serial port
    UartWrapper(Uart &port)
        : _port(port) {}

    /// @brief Propagation for read method
    /// @param minSize bytes to read
    /// @param msTimeout timeout in ms
    /// @return read bytes
    std::vector<uint8_t> read(size_t minSize = 0, unsigned msTimeout = 0) override {
        return _port.readBytes(minSize, msTimeout);
    }

    /// @brief Propagation for write method
    /// @param c character to write
    virtual void write(char c) override {
        _port.write(c);
    }

    /// @brief Propagation for write method
    /// @param str string to write
    virtual void write(std::string str) override {
        _port.write(str);
    }

  private:
    Uart &_port;
};

/**
 * @brief Enumerator for page-ids
 */
enum class Page {
    Splash = 0, ///< Splash-screen
    Working,    ///< Working-screen
    Info        ///< Info-page
};

/**
 * @brief Base class for pages
 * 
 */
class PageBase {
  public:
    /// @brief Type for event callback
    using EventCallback = void (*)(void *);
    /// @brief Type for event arguments
    using BArgs = void *;
    /// @brief Type for bind callback
    using BindCb = std::function<void(BArgs)>;
    /// @brief Type for switch page callback
    using SwitchCb = std::function<void(display::PageBase *)>;

    /// @brief Constructor
    /// @param display Reference to display
    /// @param id PageId
    /// @param name Page-name
    /// @param genericCb Generic event callback
    PageBase(Nextion &display, Page id, std::string_view name, EventCallback genericCb)
        : _display(display),
          _page(display.createPage(static_cast<uint8_t>(id), name)),
          _genericCb(genericCb) {}

    /// @brief Method for initial call
    virtual void init(){};

    /// @brief Method for cyclic calls
    virtual void tick(){};

    /// @brief create void functional pointer from functional object
    /// @param cb functional callback
    /// @return void pointer
    static void *createFunctional(BindCb cb) {
        BindCb *c = new BindCb{cb};
        return (void *)c;
    }

    /// @brief Display current page
    /// @return Id of active page
    PageBase *show() {
        _page.show();
        return this;
    }

    /// @brief Get page name
    /// @return name of page
    std::string getName() {
        return _page.getObjName();
    }

  protected:
    Nextion &_display;
    nxt::Page _page;
    EventCallback _genericCb;
};

/**
 * @brief Definition of splash-screen / welcome-page
 */
class SplashScreenPage : public PageBase {
  public:
    /// @brief Constructor
    /// @param display Reference to display
    /// @param genericCb Generic event callback
    SplashScreenPage(Nextion &display, EventCallback genericCb)
        : PageBase(display, Page::Splash, "splashScreen", genericCb),
          tHealth(_page.createComponent<nxt::Text>(4, "tHealth")) {}

    /// @brief Text-field for health indication
    nxt::Text tHealth;

    /// @brief Specific init
    void init() override {
        tHealth.text.set("Setup Image");
    }
};

/**
 * @brief Definition of working-page
 */
class WorkingPage : public PageBase {
  public:
    /// @brief Type for color callback
    using setColorCallback = void (*)(AmbientColorSet);

    /// @brief Constructor
    /// @param display Reference to display
    /// @param genericCb Generic event callback
    /// @param presets Reference to color presets
    /// @param pageSwitch Callback for switching to different page
    /// @param colorCb Callback to color switch
    WorkingPage(Nextion &display, EventCallback genericCb, ColorPresets &presets, SwitchCb pageSwitch,
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
          _colorPresets(presets), _pageSwitchCb(pageSwitch) {
        auto gCb = _genericCb;
        auto cF = createFunctional;
        bScheme1.attachPush(gCb, cF([this](BArgs a) { _colorSetCb(_colorPresets[0]); }));
        bScheme2.attachPush(gCb, cF([this](BArgs a) { _colorSetCb(_colorPresets[1]); }));
        bScheme3.attachPush(gCb, cF([this](BArgs a) { _colorSetCb(_colorPresets[2]); }));
        bSchemeCustom.attachPush(gCb, cF([this](BArgs a) { _colorSetCb(_customPreset); }));
        hCustomFg.attachPop(gCb, cF([this](BArgs a) { this->hCustomFgCb(a); }));
        hCustomBg.attachPop(gCb, cF([this](BArgs a) { this->hCustomBgCb(a); }));
        bToInfoPage.attachPush(gCb, cF([this](BArgs a) { this->switchPage(a); }));
    }

    /// @brief Button for color scheme 1
    nxt::Button bScheme1;
    /// @brief Button for color scheme 2
    nxt::Button bScheme2;
    /// @brief Button for color scheme 3
    nxt::Button bScheme3;
    /// @brief Button for custom color scheme
    nxt::Button bSchemeCustom;
    /// @brief Button to switch to info-page
    nxt::Button bToInfoPage;

    /// @brief Color field as visualisation foreground of scheme 1
    nxt::Text tScheme1Fg;
    /// @brief Color field as visualisation background of scheme 1
    nxt::Text tScheme1Bg;
    /// @brief Color field as visualisation foreground of scheme 2
    nxt::Text tScheme2Fg;
    /// @brief Color field as visualisation background of scheme 2
    nxt::Text tScheme2Bg;
    /// @brief Color field as visualisation foreground of scheme 3
    nxt::Text tScheme3Fg;
    /// @brief Color field as visualisation background of scheme 3
    nxt::Text tScheme3Bg;
    /// @brief Color field as visualisation foreground of custom scheme
    nxt::Text tCustomFg;
    /// @brief Color field as visualisation background of custom scheme
    nxt::Text tCustomBg;

    /// @brief Slider for custom foreground hue
    nxt::Slider hCustomFg;
    /// @brief Slider for custom background hue
    nxt::Slider hCustomBg;
    /// @brief Visualisation of light intensity
    nxt::ProgressBar jLight;
    /// @brief Visualisation of background intensity
    nxt::ProgressBar jAmbient;

    /// @brief Specific init
    void init() override {
        auto &ic = _colorPresets;
        tScheme1Fg.background.set(calcColor(ic[0].foregroundColor));
        tScheme1Bg.background.set(calcColor(ic[0].backgroundColor));
        tScheme2Fg.background.set(calcColor(ic[1].foregroundColor));
        tScheme2Bg.background.set(calcColor(ic[1].backgroundColor));
        tScheme3Fg.background.set(calcColor(ic[2].foregroundColor));
        tScheme3Bg.background.set(calcColor(ic[2].backgroundColor));

        auto fg = hCustomFg.value.get();
        tCustomFg.background.set(calcColor(Color::fromHue(fg)));
        auto bg = hCustomBg.value.get();
        tCustomBg.background.set(calcColor(Color::fromHue(bg)));

        jLight.value.set(0);
        jAmbient.value.set(0);
    }

    /// @brief Specific cyclic task
    void tick() override {
        _display.nexLoop(_sensingList);
    }

    /// @brief Callback for changing hue of custom foreground
    /// @param ptr ignored argument
    void hCustomFgCb(void *ptr) {
        auto fg = hCustomFg.value.get();
        auto color = Color::fromHue(fg);
        _customPreset.foregroundColor = color;
        tCustomFg.background.set(calcColor(color));
    }

    /// @brief Callback for changing hue of custom background
    /// @param ptr ignored argument
    void hCustomBgCb(void *ptr) {
        auto bg = hCustomBg.value.get();
        auto color = Color::fromHue(bg);
        _customPreset.backgroundColor = color;
        tCustomBg.background.set(calcColor(color));
    }

    /// @brief Switch to this page
    void switchPage(void *ptr) {
        _pageSwitchCb(this);
    }

    /// @brief Generate color code from color object
    /// @param col color object
    /// @return color code
    static uint32_t calcColor(Color col) {
        return nxt::Color::calcColor(col.red, col.green, col.blue);
    }

  private:
    setColorCallback _colorSetCb;
    ColorPresets &_colorPresets;
    SwitchCb _pageSwitchCb;

    AmbientColorSet _customPreset;
    Nextion::SensingList _sensingList = {&bToInfoPage, &bScheme1, &bScheme2, &bScheme3,
                                         &bSchemeCustom, &hCustomFg, &hCustomBg};
};

/**
 * @brief Definition of info-page
 */
class InfoPage : public PageBase {
  public:
    /// @brief Constructor
    /// @param display Reference to display
    /// @param genericCb Generic event callback
    /// @param name device name for display
    /// @param version device version for display
    /// @param pageSwitch Callback for switching to different page
    InfoPage(Nextion &display, EventCallback genericCb, const std::string &name, const std::string &version, SwitchCb pageSwitch)
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

    /// @brief Button to switch to working-page
    nxt::Button bToWorkingPage;
    /// @brief Text field for device-name
    nxt::Text tName;
    /// @brief Text field for version-string
    nxt::Text tVersion;
    /// @brief Text field for IP-address
    nxt::Text tAddress;
    /// @brief Text field for device infos
    nxt::Text tInfo;
    /// @brief Text field for device status
    nxt::Text tStatus;

    /// @brief Specific init
    void init() override {
        tName.text.set(_name);
        tVersion.text.set(_version);
        tAddress.text.set("0.0.0.0");
        tInfo.text.set("Device Info");
        tStatus.text.set("Device Status");
    }

    /// @brief Switch to this page
    void switchPage(void *ptr) {
        _pageSwitchCb(this);
    }

    /// @brief Specific cyclic task
    void tick() override {
        _display.nexLoop(_sensingList);
    }

  private:
    const std::string &_name;
    const std::string &_version;
    SwitchCb _pageSwitchCb;
    Nextion::SensingList _sensingList = {&bToWorkingPage};
};

} // namespace display
