#include "NexPage.h"
#include "NexText.h"
#include "NexNumber.h"
#include "NexScrolltext.h"
#include "NexButton.h"
#include "Streams.hpp"
#include "fakes/streamDummy.hpp"
#include <gtest/gtest.h>

StreamDummy stream;

TEST(DisplayTestElements, PageTests) {
    nexInit(stream);
    NexPage eut{0, 1, "eut"};
    
    eut.show();
    EXPECT_EQ(stream.lastWrite, std::string("page eut") + EndSequence);
}

TEST(DisplayTestElements, ColorTests) {
    auto r = NexColor::calcNextionColor(255,0,0);
    EXPECT_EQ(r, 63488);
    auto g = NexColor::calcNextionColor(0,255,0);
    EXPECT_EQ(g, 2016);
    auto b = NexColor::calcNextionColor(0,0,255);
    EXPECT_EQ(b, 31);
}

TEST(DisplayTestElements, TextTests) {
    nexInit(stream);
    NexText eut(0, 1, "eut");

    eut.getText();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.txt") + EndSequence);
    eut.setText("xyz");
    EXPECT_EQ(stream.lastWrite, std::string("eut.txt=\"xyz\"") + EndSequence);

    eut.background.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.bco") + EndSequence);
    eut.background.setColor(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.bco=123") + EndSequence);
    eut.fontColor.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pco") + EndSequence);
    eut.fontColor.setColor(234);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pco=234") + EndSequence);

    eut.alignment.getPlaceXcen();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.xcen") + EndSequence);
    eut.alignment.setPlaceXcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.xcen=123") + EndSequence);
    eut.alignment.getPlaceYcen();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.ycen") + EndSequence);
    eut.alignment.setPlaceYcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.ycen=123") + EndSequence);

    eut.font.getFont();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.font") + EndSequence);
    eut.font.setFont(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.font=123") + EndSequence);

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.picc") + EndSequence);
    eut.backgroundImage.setBackgroundCropPicc(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.picc=123") + EndSequence);
    eut.backgroundImage.getImagePic();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pic") + EndSequence);
    eut.backgroundImage.setImagePic(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pic=123") + EndSequence);
}

TEST(DisplayTestElements, NumberTests) {
    nexInit(stream);
    NexNumber eut(0, 1, "eut");

    eut.getValue();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.val") + EndSequence);
    eut.setValue(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.val=123") + EndSequence);

    eut.background.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.bco") + EndSequence);
    eut.background.setColor(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.bco=123") + EndSequence);
    eut.fontColor.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pco") + EndSequence);
    eut.fontColor.setColor(234);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pco=234") + EndSequence);

    eut.alignment.getPlaceXcen();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.xcen") + EndSequence);
    eut.alignment.setPlaceXcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.xcen=123") + EndSequence);
    eut.alignment.getPlaceYcen();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.ycen") + EndSequence);
    eut.alignment.setPlaceYcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.ycen=123") + EndSequence);

    eut.font.getFont();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.font") + EndSequence);
    eut.font.setFont(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.font=123") + EndSequence);

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.picc") + EndSequence);
    eut.backgroundImage.setBackgroundCropPicc(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.picc=123") + EndSequence);
    eut.backgroundImage.getImagePic();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pic") + EndSequence);
    eut.backgroundImage.setImagePic(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pic=123") + EndSequence);
}

TEST(DisplayTestElements, ScrollTextTests) {
    nexInit(stream);
    NexScrolltext eut(0, 1, "eut");

    eut.getText();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.txt") + EndSequence);
    eut.setText("xyz");
    EXPECT_EQ(stream.lastWrite, std::string("eut.txt=\"xyz\"") + EndSequence);

    eut.background.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.bco") + EndSequence);
    eut.background.setColor(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.bco=123") + EndSequence);
    eut.fontColor.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pco") + EndSequence);
    eut.fontColor.setColor(234);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pco=234") + EndSequence);

    eut.alignment.getPlaceXcen();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.xcen") + EndSequence);
    eut.alignment.setPlaceXcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.xcen=123") + EndSequence);
    eut.alignment.getPlaceYcen();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.ycen") + EndSequence);
    eut.alignment.setPlaceYcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.ycen=123") + EndSequence);

    eut.font.getFont();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.font") + EndSequence);
    eut.font.setFont(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.font=123") + EndSequence);

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.picc") + EndSequence);
    eut.backgroundImage.setBackgroundCropPicc(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.picc=123") + EndSequence);
    eut.backgroundImage.getImagePic();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pic") + EndSequence);
    eut.backgroundImage.setImagePic(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pic=123") + EndSequence);


    eut.Get_scroll_dir();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.dir") + EndSequence);
    eut.Set_scroll_dir(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.dir=123") + EndSequence);
    eut.Get_scroll_distance();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.dis") + EndSequence);
    eut.Set_scroll_distance(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.dis=123") + EndSequence);
    eut.Get_cycle_tim();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.tim") + EndSequence);
    eut.Set_cycle_tim(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.tim=123") + EndSequence);

    eut.enable();
    EXPECT_EQ(stream.lastWrite, std::string("eut.en=1") + EndSequence);
    eut.disable();
    EXPECT_EQ(stream.lastWrite, std::string("eut.en=0") + EndSequence);
}

TEST(DisplayTestElements, ButtonTests) {
    nexInit(stream);
    NexButton eut(0, 1, "eut");

    eut.getText();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.txt") + EndSequence);
    eut.setText("xyz");
    EXPECT_EQ(stream.lastWrite, std::string("eut.txt=\"xyz\"") + EndSequence);

    eut.background.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.bco") + EndSequence);
    eut.background.setColor(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.bco=123") + EndSequence);
    eut.fontColor.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pco") + EndSequence);
    eut.fontColor.setColor(234);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pco=234") + EndSequence);

    eut.backgroundPressed.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.bco2") + EndSequence);
    eut.backgroundPressed.setColor(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.bco2=123") + EndSequence);
    eut.fontColorPressed.getColor();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pco2") + EndSequence);
    eut.fontColorPressed.setColor(234);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pco2=234") + EndSequence);

    eut.alignment.getPlaceXcen();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.xcen") + EndSequence);
    eut.alignment.setPlaceXcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.xcen=123") + EndSequence);
    eut.alignment.getPlaceYcen();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.ycen") + EndSequence);
    eut.alignment.setPlaceYcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.ycen=123") + EndSequence);

    eut.font.getFont();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.font") + EndSequence);
    eut.font.setFont(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.font=123") + EndSequence);

    eut.backgroundImage.getBackgroundCropPicc();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.picc") + EndSequence);
    eut.backgroundImage.setBackgroundCropPicc(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.picc=123") + EndSequence);
    eut.backgroundImage.getImagePic();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pic") + EndSequence);
    eut.backgroundImage.setImagePic(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pic=123") + EndSequence);

    eut.backgroundImagePressed.getBackgroundCropPicc();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.picc2") + EndSequence);
    eut.backgroundImagePressed.setBackgroundCropPicc(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.picc2=123") + EndSequence);
    eut.backgroundImagePressed.getImagePic();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pic2") + EndSequence);
    eut.backgroundImagePressed.setImagePic(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pic2=123") + EndSequence);
}

