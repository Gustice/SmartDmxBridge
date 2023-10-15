#include "ScaledValue.hpp"
#include <gtest/gtest.h>


class ScaledValueTest : public ::testing::Test {
 protected:
  // void SetUp() override {}
  // void TearDown() override {}
  ScaledValue<int> sut {{0,10}, {0,100}};
};

// Test
TEST_F(ScaledValueTest, GetBottomEnd) {
  EXPECT_EQ(sut.scale(0), 0);
}

TEST_F(ScaledValueTest, GetTopEnd) {
  EXPECT_EQ(sut.scale(10), 100);
}

TEST_F(ScaledValueTest, GetArbitraryValues) {
  EXPECT_EQ(sut.scale(2), 20);
  EXPECT_EQ(sut.scale(5), 50);
  EXPECT_EQ(sut.scale(8), 80);
}

TEST_F(ScaledValueTest, AlternativeConfiguration1) {
  ScaledValue<int> sut {{5,10}, {0,100}};

  EXPECT_EQ(sut.scale(5), 0);
  EXPECT_EQ(sut.scale(10), 100);
  
  EXPECT_EQ(sut.scale(8), 60);
}

TEST_F(ScaledValueTest, AlternativeConfiguration2) {
  ScaledValue<int> sut {{-10,10}, {0,100}};

  EXPECT_EQ(sut.scale(-10), 0);
  EXPECT_EQ(sut.scale(10), 100);
  
  EXPECT_EQ(sut.scale(0), 50);
}

