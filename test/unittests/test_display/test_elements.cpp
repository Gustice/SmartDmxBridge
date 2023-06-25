#include "NexPage.h"
#include "NexText.h"
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

TEST(DisplayTestElements, TextTests) {
    uint32_t num;

    nexInit(stream);
    NexText eut(0, 1, "eut");

    eut.getText();
    EXPECT_EQ(stream.lastWrite, std::string("get eut.txt") + EndSequence);

    eut.setText("xyz");
    EXPECT_EQ(stream.lastWrite, std::string("eut.txt=\"xyz\"") + EndSequence);

    eut.Get_background_color_bco(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.bco") + EndSequence);
    
    eut.Set_background_color_bco(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.bco=123") + EndSequence);

    eut.Get_font_color_pco(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.pco") + EndSequence);
    
    eut.Set_font_color_pco(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.pco=123") + EndSequence);

    eut.Get_place_xcen(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.xcen") + EndSequence);
    
    eut.Set_place_xcen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.xcen=123") + EndSequence);

    eut.Get_place_ycen(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.ycen") + EndSequence);
    
    eut.Set_place_ycen(123);
    EXPECT_EQ(stream.lastWrite, std::string("eut.ycen=123") + EndSequence);

    eut.getFont(&num);
    EXPECT_EQ(stream.lastWrite, std::string("get eut.font") + EndSequence);
    
    eut.setFont(123);
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
    
}