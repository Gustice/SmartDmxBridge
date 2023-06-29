#include "Nextion.h"
#include "Streams.hpp"
#include "fakes/streamDummy.hpp"
#include <gtest/gtest.h>

StreamDummy stream;
auto &lW = stream.lastWrite;

std::string aESeq(std::string_view sv) {
    return std::string{sv.begin()} + EndSequence;
}

TEST(DisplayTestElements, PageTests) {
    nexInit(stream);
    NexPage eut{0, 1, "eut"};

    eut.show();
    EXPECT_EQ(lW, aESeq("page eut"));
}

TEST(DisplayTestElements, ColorTests) {
    auto r = Nxt::Color::calcNextionColor(255, 0, 0);
    EXPECT_EQ(r, 63488);
    auto g = Nxt::Color::calcNextionColor(0, 255, 0);
    EXPECT_EQ(g, 2016);
    auto b = Nxt::Color::calcNextionColor(0, 0, 255);
    EXPECT_EQ(b, 31);
}

class Pseudo : public NexTouch {
  public:
    Pseudo(uint8_t pid, uint8_t cid, const char *name) : NexTouch(pid, cid, name) {}

    Nxt::TextValue text{*this, "txt"};
    Nxt::IntegerValue value{*this, "val"};
    Nxt::LimitedIntegerValue limValue{*this, "lVal", 5};
    Nxt::Color color{*this, "col"};
    Nxt::Alignment alignment{*this};
    Nxt::Font font{*this};
    Nxt::BackgroundImage backgroundImage{*this};
};

TEST(DisplayTestElements, TestPropertyBases) {
    nexInit(stream);
    Pseudo eut(0, 1, "eut");

    eut.text.get();
    EXPECT_EQ(lW, aESeq("get eut.txt"));
    eut.text.set("xyz");
    EXPECT_EQ(lW, aESeq("eut.txt=\"xyz\""));

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));
    eut.value.set(123);
    EXPECT_EQ(lW, aESeq("eut.val=123"));

    eut.limValue.get();
    EXPECT_EQ(lW, aESeq("get eut.lVal"));
    eut.limValue.set(123);
    EXPECT_EQ(lW, aESeq("eut.lVal=123"));
    eut.limValue.set(0);
    EXPECT_EQ(lW, aESeq("eut.lVal=5"));

    eut.color.get();
    EXPECT_EQ(lW, aESeq("get eut.col"));
    eut.color.set(123);
    EXPECT_EQ(lW, aESeq("eut.col=123"));
    eut.color.getColor();
    EXPECT_EQ(lW, aESeq("get eut.col"));
    eut.color.setColor(123);
    EXPECT_EQ(lW, aESeq("eut.col=123"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(lW, aESeq("get eut.xcen"));
    eut.alignment.placeXcen.set(123);
    EXPECT_EQ(lW, aESeq("eut.xcen=123"));
    eut.alignment.placeYcen.get();
    EXPECT_EQ(lW, aESeq("get eut.ycen"));
    eut.alignment.placeYcen.set(123);
    EXPECT_EQ(lW, aESeq("eut.ycen=123"));

    eut.font.getFont();
    EXPECT_EQ(lW, aESeq("get eut.font"));
    eut.font.setFont(123);
    EXPECT_EQ(lW, aESeq("eut.font=123"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(lW, aESeq("get eut.picc"));
    eut.backgroundImage.setBackgroundCropPicc(123);
    EXPECT_EQ(lW, aESeq("eut.picc=123"));
    eut.backgroundImage.getImagePic();
    EXPECT_EQ(lW, aESeq("get eut.pic"));
    eut.backgroundImage.setImagePic(123);
    EXPECT_EQ(lW, aESeq("eut.pic=123"));
}

TEST(DisplayTestElements, TextTests) {
    nexInit(stream);
    Nxt::Text eut(0, 1, "eut");

    // Testing only Getter
    eut.text.get();
    EXPECT_EQ(lW, aESeq("get eut.txt"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(lW, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(lW, aESeq("get eut.font"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(lW, aESeq("get eut.picc"));
}

TEST(DisplayTestElements, NumberTests) {
    nexInit(stream);
    Nxt::Number eut(0, 1, "eut");

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(lW, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(lW, aESeq("get eut.font"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(lW, aESeq("get eut.picc"));
}

TEST(DisplayTestElements, ScrollTextTests) {
    nexInit(stream);
    Nxt::Scrolltext eut(0, 1, "eut");

    eut.text.get();
    EXPECT_EQ(lW, aESeq("get eut.txt"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(lW, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(lW, aESeq("get eut.font"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(lW, aESeq("get eut.picc"));

    eut.scrollDir.get();
    EXPECT_EQ(lW, aESeq("get eut.dir"));
    eut.scrollDistance.get();
    EXPECT_EQ(lW, aESeq("get eut.dis"));
    eut.cycleTime.get();
    EXPECT_EQ(lW, aESeq("get eut.tim"));

    eut.enable();
    EXPECT_EQ(lW, aESeq("eut.en=1"));
    eut.disable();
    EXPECT_EQ(lW, aESeq("eut.en=0"));
}

TEST(DisplayTestElements, ButtonTests) {
    nexInit(stream);
    Nxt::Button eut(0, 1, "eut");

    eut.text.get();
    EXPECT_EQ(lW, aESeq("get eut.txt"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));

    eut.backgroundPressed.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco2"));
    eut.fontColorPressed.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco2"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(lW, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(lW, aESeq("get eut.font"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(lW, aESeq("get eut.picc"));

    eut.backgroundImagePressed.getBackgroundCropPicc();
    EXPECT_EQ(lW, aESeq("get eut.picc2"));
}

TEST(DisplayTestElements, SliderTests) {
    nexInit(stream);
    Nxt::Slider eut(0, 1, "eut");

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));

    eut.pointerThickness.get();
    EXPECT_EQ(lW, aESeq("get eut.wid"));
    eut.cursorHeight.get();
    EXPECT_EQ(lW, aESeq("get eut.hig"));
    eut.maxValue.get();
    EXPECT_EQ(lW, aESeq("get eut.maxval"));
    eut.minValue.get();
    EXPECT_EQ(lW, aESeq("get eut.minval"));
}

TEST(DisplayTestElements, GaugeTests) {
    nexInit(stream);
    Nxt::Gauge eut(0, 1, "eut");

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));

    eut.pointerThickness.get();
    EXPECT_EQ(lW, aESeq("get eut.wid"));

    eut.backgroundCropImage.get();
    EXPECT_EQ(lW, aESeq("get eut.picc"));
}

TEST(DisplayTestElements, VariableTest) {
    nexInit(stream);
    Nxt::Variable eut(0, 1, "eut");

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));
    eut.text.get();
    EXPECT_EQ(lW, aESeq("get eut.txt"));
}

TEST(DisplayTestElements, PictureTest) {
    nexInit(stream);
    Nxt::Picture eut(0, 1, "eut");

    eut.picture.get();
    EXPECT_EQ(lW, aESeq("get eut.pic"));
}

TEST(DisplayTestElements, CropTest) {
    nexInit(stream);
    Nxt::Crop eut(0, 1, "eut");

    eut.cropImage.get();
    EXPECT_EQ(lW, aESeq("get eut.picc"));
}

TEST(DisplayTestElements, RadioTest) {
    nexInit(stream);
    Nxt::Radio eut(0, 1, "eut");

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));
}

TEST(DisplayTestElements, CheckBoxTest) {
    nexInit(stream);
    Nxt::Checkbox eut(0, 1, "eut");

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));
}

TEST(DisplayTestElements, ProgressBarTest) {
    nexInit(stream);
    Nxt::ProgressBar eut(0, 1, "eut");

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.pco"));
}

TEST(DisplayTestElements, TimerTest) {
    nexInit(stream);
    Nxt::Timer eut(0, 1, "eut");

    eut.cycleTime.get();
    EXPECT_EQ(lW, aESeq("get eut.tim"));

    eut.enable();
    EXPECT_EQ(lW, aESeq("eut.en=1"));

    eut.disable();
    EXPECT_EQ(lW, aESeq("eut.en=0"));
}

TEST(DisplayTestElements, HotspotTest) {
    nexInit(stream);
    Nxt::Hotspot eut(0, 1, "eut");
}

TEST(DisplayTestElements, DualStateTest) {
    nexInit(stream);
    Nxt::DSButton eut(0, 1, "eut");

    eut.value.get();
    EXPECT_EQ(lW, aESeq("get eut.val"));
    eut.text.get();
    EXPECT_EQ(lW, aESeq("get eut.txt"));

    eut.fontColor.get();
    EXPECT_EQ(lW, aESeq("get eut.pco"));

    eut.colorState0.get();
    EXPECT_EQ(lW, aESeq("get eut.bco0"));
    eut.colorState1.get();
    EXPECT_EQ(lW, aESeq("get eut.bco1"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(lW, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(lW, aESeq("get eut.font"));

    eut.imageState1.getBackgroundCropPicc();
    EXPECT_EQ(lW, aESeq("get eut.picc0"));
    eut.imageState2.getBackgroundCropPicc();
    EXPECT_EQ(lW, aESeq("get eut.picc1"));
}


TEST(DisplayTestElements, RtcTest) {
    nexInit(stream);
    Nxt::Rtc eut;

    // eut.write_rtc_time("0102/03/04 05:06:07");
    // EXPECT_EQ(lW, aESeq("rtc5=07"));
    // uint32_t values[] = {0102, 03, 04, 05, 06, 07};
    // eut.write_rtc_time(values);
    // EXPECT_EQ(lW, aESeq("rtc5=07"));
    // eut.write_rtc_time("sec", 07);
    // EXPECT_EQ(lW, aESeq("rtc5=07"));

    // char time_buf[22];
    // eut.read_rtc_time(time_buf, 22);
    // EXPECT_EQ(lW, aESeq("get rtc6"));

    // eut.read_rtc_time(time_buf, 22);
    // EXPECT_EQ(lW, aESeq("get rtc6"));
}

TEST(DisplayTestElements, WaveformTest) {
    nexInit(stream);
    Nxt::Waveform eut(0, 1, "eut");

    eut.backgroundColor.get();
    EXPECT_EQ(lW, aESeq("get eut.bco"));
    eut.gridColor.getColor();
    EXPECT_EQ(lW, aESeq("get eut.gdc"));
    eut.channelColor0.get();
    EXPECT_EQ(lW, aESeq("get eut.pco0"));


    eut.gridWidth.get();
    EXPECT_EQ(lW, aESeq("get eut.gdw"));
    eut.gridHeight.get();
    EXPECT_EQ(lW, aESeq("get eut.gdh"));

    eut.addValue(2, 3);
    EXPECT_EQ(lW, aESeq("add 1,2,3"));
}

TEST(DisplayTestElements, GpioTest) {
    nexInit(stream);
    Nxt::Gpio eut;

    eut.pin_mode(1, 2, 3);
    EXPECT_EQ(lW, aESeq("cfgpio 1,2,3"));

    eut.digital_write(1,2);
    EXPECT_EQ(lW, aESeq("pio1=2"));

    eut.digital_read(1);
    EXPECT_EQ(lW, aESeq("get pio1"));

    eut.analog_write(1,2);
    EXPECT_EQ(lW, aESeq("pwm1=2"));

    eut.set_pwmfreq(10);
    EXPECT_EQ(lW, aESeq("pwmf=10"));

    eut.get_pwmfreq();
    EXPECT_EQ(lW, aESeq("get pwmf"));
}
