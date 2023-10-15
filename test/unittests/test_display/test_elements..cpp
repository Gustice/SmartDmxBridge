#include "Nextion.hpp"
#include "fakes/streamDummy.hpp"
#include <gtest/gtest.h>

using namespace nxt;
static const std::string EndSequence = "\xFF\xFF\xFF";
static void logCallback(LogSeverity level, std::string log) {
    // _lastLog = log;
}
static std::string aESeq(std::string_view sv) {
    return std::string{sv.begin()} + EndSequence;
}

// TestFixture:
class NextionElementsTest : public ::testing::Test {
  protected:
    void SetUp() override {
    }
    void TearDown() override {
    }

    NextionElementsTest()
        : _stream(), _eut(_stream), _page{_eut.createPage(0, "eut")}, _lWr(_stream.lastWrite) {
            _stream.appendNextRead(Return::InstructionSuccessful, {});
            _page.show();
        }

    StreamDummy _stream;
    Nextion _eut;
    nxt::Page _page;
    static std::string _lastLog;
    std::string &_lWr;
};
std::string NextionElementsTest::_lastLog;

TEST_F(NextionElementsTest, PageTests) {
    _page.show();
    EXPECT_EQ(_lWr, aESeq("page eut"));
}

TEST_F(NextionElementsTest, TextTests) {
    auto eut = _page.createComponent<nxt::Text>(1, "eut");

    // Testing only Getter
    eut.text.get();
    EXPECT_EQ(_lWr, aESeq("get eut.txt"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(_lWr, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(_lWr, aESeq("get eut.font"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(_lWr, aESeq("get eut.picc"));
}

TEST_F(NextionElementsTest, NumberTests) {
    auto eut = _page.createComponent<nxt::Number>(1, "eut");

    eut.value.get();
    EXPECT_EQ(_lWr, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(_lWr, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(_lWr, aESeq("get eut.font"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(_lWr, aESeq("get eut.picc"));
}

TEST_F(NextionElementsTest, ScrollTextTests) {
    auto eut = _page.createComponent<nxt::Scrolltext>(1, "eut");

    eut.text.get();
    EXPECT_EQ(_lWr, aESeq("get eut.txt"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(_lWr, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(_lWr, aESeq("get eut.font"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(_lWr, aESeq("get eut.picc"));

    eut.scrollDir.get();
    EXPECT_EQ(_lWr, aESeq("get eut.dir"));
    eut.scrollDistance.get();
    EXPECT_EQ(_lWr, aESeq("get eut.dis"));
    eut.cycleTime.get();
    EXPECT_EQ(_lWr, aESeq("get eut.tim"));

    eut.enable();
    EXPECT_EQ(_lWr, aESeq("eut.en=1"));
    eut.disable();
    EXPECT_EQ(_lWr, aESeq("eut.en=0"));
}

TEST_F(NextionElementsTest, ButtonTests) {
    auto eut = _page.createComponent<nxt::Button>(1, "eut");

    eut.text.get();
    EXPECT_EQ(_lWr, aESeq("get eut.txt"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));

    eut.backgroundPressed.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco2"));
    eut.fontColorPressed.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco2"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(_lWr, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(_lWr, aESeq("get eut.font"));

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(_lWr, aESeq("get eut.picc"));

    eut.backgroundImagePressed.getBackgroundCropPicc();
    EXPECT_EQ(_lWr, aESeq("get eut.picc2"));
}

TEST_F(NextionElementsTest, SliderTests) {
    auto eut = _page.createComponent<nxt::Slider>(1, "eut");

    eut.value.get();
    EXPECT_EQ(_lWr, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));

    eut.pointerThickness.get();
    EXPECT_EQ(_lWr, aESeq("get eut.wid"));
    eut.cursorHeight.get();
    EXPECT_EQ(_lWr, aESeq("get eut.hig"));
    eut.maxValue.get();
    EXPECT_EQ(_lWr, aESeq("get eut.maxval"));
    eut.minValue.get();
    EXPECT_EQ(_lWr, aESeq("get eut.minval"));
}

TEST_F(NextionElementsTest, GaugeTests) {
    auto eut = _page.createComponent<nxt::Gauge>(1, "eut");

    eut.value.get();
    EXPECT_EQ(_lWr, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));

    eut.pointerThickness.get();
    EXPECT_EQ(_lWr, aESeq("get eut.wid"));

    eut.backgroundCropImage.get();
    EXPECT_EQ(_lWr, aESeq("get eut.picc"));
}

TEST_F(NextionElementsTest, VariableTest) {
    auto eut = _page.createComponent<nxt::Variable>(1, "eut");

    eut.value.get();
    EXPECT_EQ(_lWr, aESeq("get eut.val"));
    eut.text.get();
    EXPECT_EQ(_lWr, aESeq("get eut.txt"));
}

TEST_F(NextionElementsTest, PictureTest) {
    auto eut = _page.createComponent<nxt::Picture>(1, "eut");

    eut.picture.get();
    EXPECT_EQ(_lWr, aESeq("get eut.pic"));
}

TEST_F(NextionElementsTest, CropTest) {
    auto eut = _page.createComponent<nxt::Crop>(1, "eut");

    eut.cropImage.get();
    EXPECT_EQ(_lWr, aESeq("get eut.picc"));
}

TEST_F(NextionElementsTest, RadioTest) {
    auto eut = _page.createComponent<nxt::Radio>(1, "eut");

    eut.value.get();
    EXPECT_EQ(_lWr, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));
}

TEST_F(NextionElementsTest, CheckBoxTest) {
    auto eut = _page.createComponent<nxt::Checkbox>(1, "eut");

    eut.value.get();
    EXPECT_EQ(_lWr, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));
}

TEST_F(NextionElementsTest, ProgressBarTest) {
    auto eut = _page.createComponent<nxt::ProgressBar>(1, "eut");

    eut.value.get();
    EXPECT_EQ(_lWr, aESeq("get eut.val"));

    eut.background.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.fontColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));
}

TEST_F(NextionElementsTest, TimerTest) {
    auto eut = _page.createComponent<nxt::Timer>(1, "eut");

    eut.cycleTime.get();
    EXPECT_EQ(_lWr, aESeq("get eut.tim"));

    eut.enable();
    EXPECT_EQ(_lWr, aESeq("eut.en=1"));

    eut.disable();
    EXPECT_EQ(_lWr, aESeq("eut.en=0"));
}

TEST_F(NextionElementsTest, HotspotTest) {
    auto eut = _page.createComponent<nxt::Hotspot>(1, "eut");
}

TEST_F(NextionElementsTest, DualStateTest) {
    auto eut = _page.createComponent<nxt::DualStateButton>(1, "eut");

    eut.value.get();
    EXPECT_EQ(_lWr, aESeq("get eut.val"));
    eut.text.get();
    EXPECT_EQ(_lWr, aESeq("get eut.txt"));

    eut.fontColor.get();
    EXPECT_EQ(_lWr, aESeq("get eut.pco"));

    eut.colorState0.get();
    EXPECT_EQ(_lWr, aESeq("get eut.bco0"));
    eut.colorState1.get();
    EXPECT_EQ(_lWr, aESeq("get eut.bco1"));

    eut.alignment.placeXcen.get();
    EXPECT_EQ(_lWr, aESeq("get eut.xcen"));

    eut.font.getFont();
    EXPECT_EQ(_lWr, aESeq("get eut.font"));

    eut.imageState1.getBackgroundCropPicc();
    EXPECT_EQ(_lWr, aESeq("get eut.picc0"));
    eut.imageState2.getBackgroundCropPicc();
    EXPECT_EQ(_lWr, aESeq("get eut.picc1"));
}

TEST_F(NextionElementsTest, WaveformTest) {
    auto eut = _page.createComponent<nxt::Waveform>(1, "eut");

    eut.backgroundColor.get();
    EXPECT_EQ(_lWr, aESeq("get eut.bco"));
    eut.gridColor.getColor();
    EXPECT_EQ(_lWr, aESeq("get eut.gdc"));
    eut.channelColor0.get();
    EXPECT_EQ(_lWr, aESeq("get eut.pco0"));

    eut.gridWidth.get();
    EXPECT_EQ(_lWr, aESeq("get eut.gdw"));
    eut.gridHeight.get();
    EXPECT_EQ(_lWr, aESeq("get eut.gdh"));

    eut.addValue(2, 3);
    EXPECT_EQ(_lWr, aESeq("add 1,2,3"));
}

TEST_F(NextionElementsTest, GpioTest) {
    nxt::Gpio &eut = _eut.getGpio();

    eut.pin_mode(1, 2, 3);
    EXPECT_EQ(_lWr, aESeq("cfgpio 1,2,3"));

    eut.digital_write(1,2);
    EXPECT_EQ(_lWr, aESeq("pio1=2"));

    eut.digital_read(1);
    EXPECT_EQ(_lWr, aESeq("get pio1"));

    eut.analog_write(1,2);
    EXPECT_EQ(_lWr, aESeq("pwm1=2"));

    eut.set_pwmfreq(10);
    EXPECT_EQ(_lWr, aESeq("pwmf=10"));

    eut.get_pwmfreq();
    EXPECT_EQ(_lWr, aESeq("get pwmf"));
}

TEST_F(NextionElementsTest, RtcTest) {
    nxt::Rtc &eut = _eut.getRtc();

    // eut.write_rtc_time("0102/03/04 05:06:07");
    // EXPECT_EQ(_lWr, aESeq("rtc5=07"));
    // uint32_t values[] = {0102, 03, 04, 05, 06, 07};
    // eut.write_rtc_time(values);
    // EXPECT_EQ(_lWr, aESeq("rtc5=07"));
    // eut.write_rtc_time("sec", 07);
    // EXPECT_EQ(_lWr, aESeq("rtc5=07"));

    // char time_buf[22];
    // eut.read_rtc_time(time_buf, 22);
    // EXPECT_EQ(_lWr, aESeq("get rtc6"));

    // eut.read_rtc_time(time_buf, 22);
    // EXPECT_EQ(_lWr, aESeq("get rtc6"));
}
