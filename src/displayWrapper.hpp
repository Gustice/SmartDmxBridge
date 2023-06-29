#pragma once

#include "Nextion.h"
#include "esp_log.h"
#include "uart.hpp"

uint32_t scaleColorDown(uint8_t val, uint8_t max) {
    return (uint8_t) ((uint32_t)val*max/0xFF);
}

uint32_t calcNextionColor(uint8_t r, uint8_t g, uint8_t b) {
    // red = 63488 / F800
    // green = 2016 /  07E0
    // blue = 31 / 1F
    return scaleColorDown(b, 0x1F) 
        & (scaleColorDown(g, 0x3F) << 5) 
        & (scaleColorDown(r, 0x1F) << (5+6));
}

class Display {
  public:
    Display(Uart &port) : _port(port) {
        nexInit(port);

        ESP_LOGI("DISP", "begin setup");
        tHealth.setText("Setup Image");

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

    }

    void tick() {
        switch (page) {
        case CurrentPage::WorkingPage: {
            NexTouch *nex_listen_list[] = {
                &bToInfoPage,   &bScheme1,  &bScheme2,  &bScheme3,
                &bSchemeCustom, &hCustomFg, &hCustomBg, nullptr,
            };
        nexLoop(nex_listen_list);

        } break;

        case CurrentPage::InfoPage: {
            NexTouch *nex_listen_list[] = {
                &bToWorkingPage,
                nullptr,
            };
        nexLoop(nex_listen_list);

        } break;

        default:
            break;
        }
    }

  private:
    enum CurrentPage { WorkingPage = 1, InfoPage, Pages };

    Uart _port;
    CurrentPage page = CurrentPage::WorkingPage;

    NexPage workingPage{1, 0, "workingPage"};
    NexPage infoPage{2, 0, "infoPage"};

    NexButton bScheme1{1, 1, "bScheme1"};
    NexButton bScheme2{1, 4, "bScheme2"};
    NexButton bScheme3{1, 7, "bScheme3"};
    NexButton bSchemeCustom{1, 10, "bSchemeCustom"};

    NexButton bToInfoPage{1, 15, "bNext"};
    NexButton bToWorkingPage{2, 11, "bPrev"};

    NexText tHealth{0, 4, "tHealth"};
    NexText tScheme1Fg{1, 2, "tScheme1Fg"};
    NexText tScheme1Bg{1, 3, "tScheme1Bg"};
    NexText tScheme2Fg{1, 5, "tScheme2Fg"};
    NexText tScheme2Bg{1, 6, "tScheme2Bg"};
    NexText tScheme3Fg{1, 8, "tScheme3Fg"};
    NexText tScheme3Bg{1, 9, "tScheme3Bg"};
    NexText tCustomFg{1, 16, "tCustomFg"};
    NexText tCustomBg{1, 17, "tCustomBg"};

    NexSlider hCustomFg{1, 1, "hCustomFg"};
    NexSlider hCustomBg{1, 1, "hCustomBg"};

    NexText tName{2, 3, "tName"};
    NexText tVersion{2, 4, "tVersion"};
    NexText tAddress{2, 6, "tAddress"};
    NexText tInfo{2, 8, "tInfo"};
    NexText tStatus{2, 10, "tStatus"};


    static void switchToWorkingPage(void *ptr) {
        Display * display = (Display*) ptr;

        display->workingPage.show();
        display->page = CurrentPage::WorkingPage;
        display->tScheme1Fg.Set_background_color_bco(calcNextionColor(0xFF,0,0));
        display->tScheme2Fg.Set_background_color_bco(calcNextionColor(0,0xFF,0));
        display->tScheme3Fg.Set_background_color_bco(calcNextionColor(0,0,0xFF));
    }
    static void switchToInfoPage(void *ptr) {
        Display * display = (Display*) ptr;

        display->infoPage.show();
        display->page = CurrentPage::InfoPage;
        display->tName.setText("DMX-Bridge");
        display->tVersion.setText("T 0.0.0");
        display->tAddress.setText("0.0.0.0");
        display->tInfo.setText("Device Info");
        display->tStatus.setText("Device Status");
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
};