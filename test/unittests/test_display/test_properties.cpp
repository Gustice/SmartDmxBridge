#include "Nextion.hpp"
#include "fakes/streamDummy.hpp"
#include <gtest/gtest.h>

static StreamDummy stream;
static nxt::Port port(stream);
static auto &lW = stream.lastWrite;

static const std::string EndSequence = "\xFF\xFF\xFF";
std::string aESeq(std::string_view sv) {
    return std::string{sv.begin()} + EndSequence;
}


class Pseudo : public nxt::Touch {
  public:
    Pseudo(nxt::Page & pid, uint8_t cid, const char *name) : nxt::Touch(pid, port, cid, name) {}

    nxt::TextValue text{*this, "txt"};
    nxt::IntegerValue value{*this, "val"};
    nxt::LimitedIntegerValue limValue{*this, "lVal", 5};
    nxt::Color color{*this, "col"};
    nxt::Alignment alignment{*this};
    nxt::Font font{*this};
    nxt::BackgroundImage backgroundImage{*this};
};

TEST(NextionElementPropertiesTest, TestPropertyBases) {
    Nextion frame(stream, nullptr);
    auto page = frame.createPage(0, "page");
    Pseudo eut(page, 1, "eut");
    stream.appendNextRead(nxt::Return::InstructionSuccessful, {});
    page.show();

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

TEST(NextionElementPropertiesTest, ColorTests) {
    auto r = nxt::Color::calcColor(255, 0, 0);
    EXPECT_EQ(r, 63488);
    auto g = nxt::Color::calcColor(0, 255, 0);
    EXPECT_EQ(g, 2016);
    auto b = nxt::Color::calcColor(0, 0, 255);
    EXPECT_EQ(b, 31);
}
