#include "NexPage.h"
#include "NexText.h"
#include "NexNumber.h"
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
    uint32_t num;

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

    eut.alignment.getPlaceXcen(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.xcen") + EndSequence);
    
    eut.alignment.setPlaceXcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.xcen=123") + EndSequence);

    eut.alignment.getPlaceYcen(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.ycen") + EndSequence);
    
    eut.alignment.setPlaceYcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.ycen=123") + EndSequence);

    eut.font.getFont();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.font") + EndSequence);
    
    eut.font.setFont(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.font=123") + EndSequence);

    eut.Get_background_crop_picc(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.picc") + EndSequence);
    
    eut.Set_background_crop_picc(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.picc=123") + EndSequence);

    eut.Get_background_image_pic(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pic") + EndSequence);
    
    eut.Set_background_image_pic(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pic=123") + EndSequence);
}

TEST(DisplayTestElements, NumberTests) {
    uint32_t num;

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

    eut.alignment.getPlaceXcen(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.xcen") + EndSequence);
    
    eut.alignment.setPlaceXcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.xcen=123") + EndSequence);

    eut.alignment.getPlaceYcen(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.ycen") + EndSequence);
    
    eut.alignment.setPlaceYcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.ycen=123") + EndSequence);

    eut.font.getFont();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.font") + EndSequence);
    
    eut.font.setFont(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.font=123") + EndSequence);

    eut.Get_background_crop_picc();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.picc") + EndSequence);
    
    eut.Set_background_crop_picc(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.picc=123") + EndSequence);

    eut.Get_background_image_pic();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pic") + EndSequence);
    
    eut.Set_background_image_pic(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pic=123") + EndSequence);

}