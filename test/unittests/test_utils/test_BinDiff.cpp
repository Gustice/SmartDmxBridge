
#include "BinDiff.hpp"
#include <gtest/gtest.h>

TEST(BinDiffTest, TestWithEqual) {
    std::vector<int> s1{1, 2, 3, 4};
    std::vector<int> s2 = s1;

    BinDiff sut(8);
    auto r = sut.compare(s1, s2);
    EXPECT_EQ(true, r.areSame);
    EXPECT_EQ("", r.diff);
}

TEST(BinDiffTest, TestStringify) {
    std::vector<int> s{0x11, 0x12, 0x13, 0x14};

    BinDiff sut(8);
    EXPECT_EQ("Blk 0: 0x11, 0x12, 0x13, 0x14, \n", sut.stringify_list(s.cbegin(), s.cend()));
    EXPECT_EQ("Blk 0: 0x11, 0x12, 0x13, 0x14, \n", sut.stringify_list(s));
}

TEST(BinDiffTest, TestWithDifferentValues) {
    std::vector<int> s1{0x11, 0x12, 0x13, 0x14};
    std::vector<int> s2{0x11, 0x12, 0x13, 0x10};

    BinDiff sut(8);
    auto r = sut.compare(s1, s2);
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("differences found ...\nBlk 0: 0x11/0x11, 0x12/0x12, 0x13/0x13, 0x14/0x10, \n",
              r.diff);

    r = sut.compare(s2, s1);
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("differences found ...\nBlk 0: 0x11/0x11, 0x12/0x12, 0x13/0x13, 0x10/0x14, \n",
              r.diff);
}

TEST(BinDiffTest, TestWithDifferentType) {
    std::vector<uint8_t> s1{0x11, 0x12, 0x13, 0x14};
    std::vector<uint8_t> s2{255, 0x0, 128, 0x0};

    BinDiff sut(8);
    EXPECT_EQ("Blk 0: 0x11, 0x12, 0x13, 0x14, \n",
              sut.stringify_list(s1));

    EXPECT_EQ("Blk 0: 0x11, 0x12, 0x13, 0x14, \n",
              sut.stringify_list(s1.cbegin(), s1.cend()));

    EXPECT_EQ("Blk 0: 0xff, 0x0, 0x80, 0x0, \n",
              sut.stringify_list(s2.cbegin(), s2.cend()));
}


TEST(BinDiffTest, TestWithDifferentSizes) {
    std::vector<int> s1{1, 2, 3, 4};
    std::vector<int> s2{1, 2, 3, 4, 5};

    BinDiff sut(8);
    auto r = sut.compare(s1, s2);
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("additional elements on right side ...\nBlk 0: 0x5, \n", r.diff);

    r = sut.compare(s2, s1);
    EXPECT_EQ(false, r.areSame);
    EXPECT_EQ("additional elements on left side ...\nBlk 0: 0x5, \n", r.diff);
}

