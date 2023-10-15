
#include "BinDiff.hpp"
#include <gtest/gtest.h>

TEST(BinDiffTest, TestWithEqual) {
    std::vector<uint8_t> s1{1, 2, 3, 4};
    std::vector<uint8_t> s2 = s1;

    BinDiff sut(8);
    auto r = sut.compareBytes(s1, s2, {"left", "right"});
    EXPECT_EQ(true, r.areSame);
    EXPECT_EQ("", r.diff);
}

TEST(BinDiffTest, TestStringify) {
    std::vector<uint8_t> s{0x11, 0x12, 0x13, 0x14};

    BinDiff sut(8);
    EXPECT_EQ("Blk 0: 0x11, 0x12, 0x13, 0x14, \n", sut.stringify_list(s.cbegin(), s.cend()));
    EXPECT_EQ("Blk 0: 0x11, 0x12, 0x13, 0x14, \n", sut.stringify_list(s));
}

TEST(BinDiffTest, TestWithDifferentValuesAtEnd) {
    std::vector<uint8_t> s1{0x11, 0x12, 0x13, 0x14};
    std::vector<uint8_t> s2{0x11, 0x12, 0x13, 0x10};

    BinDiff sut(8);
    auto r = sut.compareBytes(s1, s2, {"left", "right"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
left:  0x11121314\n\
right: 0x11121310\n\
               ##",
              r.diff);

    r = sut.compareBytes(s2, s1, {"right", "left"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
right: 0x11121310\n\
left:  0x11121314\n\
               ##",
              r.diff);
}

TEST(BinDiffTest, TestWithDifferentValuesAtStart) {
    std::vector<uint8_t> s1{0x11, 0x12, 0x13, 0x14};
    std::vector<uint8_t> s2{0x10, 0x12, 0x13, 0x14};

    BinDiff sut(8);
    auto r = sut.compareBytes(s1, s2, {"left", "right"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
left:  0x11121314\n\
right: 0x10121314\n\
         ##      ",
              r.diff);

    r = sut.compareBytes(s2, s1, {"right", "left"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
right: 0x10121314\n\
left:  0x11121314\n\
         ##      ",
              r.diff);
}

TEST(BinDiffTest, TestWithDifferentValuesAndSmallValues) {
    std::vector<uint8_t> s1{0x1, 0x2, 0x3, 0x4};
    std::vector<uint8_t> s2{0x1, 0x2, 0x3, 0x0};

    BinDiff sut(8);
    auto r = sut.compareBytes(s1, s2, {"left", "right"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
left:  0x01020304\n\
right: 0x01020300\n\
               ##",
              r.diff);

    r = sut.compareBytes(s2, s1, {"right", "left"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
right: 0x01020300\n\
left:  0x01020304\n\
               ##",
              r.diff);
}


TEST(BinDiffTest, TestWithDifferentSizes) {
    std::vector<uint8_t> s1{0x11, 0x12, 0x13, 0x14};
    std::vector<uint8_t> s2{0x11, 0x12, 0x13, 0x14, 0x15};

    BinDiff sut(8);
    auto r = sut.compareBytes(s1, s2, {"left", "right"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
left:  0x11121314\n\
right: 0x1112131415\n\
                 ##",
              r.diff);

    r = sut.compareBytes(s2, s1, {"right", "left"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
right: 0x1112131415\n\
left:  0x11121314\n\
                 ##",
              r.diff);
}

TEST(BinDiffTest, TestWithDifferentSizesAndSmallValues) {
    std::vector<uint8_t> s1{1, 2, 3, 4};
    std::vector<uint8_t> s2{1, 2, 3, 4, 5};

    BinDiff sut(8);
    auto r = sut.compareBytes(s1, s2, {"left", "right"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
left:  0x01020304\n\
right: 0x0102030405\n\
                 ##",
              r.diff);

    r = sut.compareBytes(s2, s1, {"right", "left"});
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("\
diff found ...\n\
right: 0x0102030405\n\
left:  0x01020304\n\
                 ##",
              r.diff);
}
