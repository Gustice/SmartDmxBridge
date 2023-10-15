#include "ValueCache.hpp"
#include <gtest/gtest.h>

TEST(ValueCacheTest, CheckInitialState) {
  ValueCache<int> sut;
  EXPECT_EQ(true, sut.isNew());
  EXPECT_EQ(0, sut.getValue());
}

TEST(ValueCacheTest, ValueResetsToOldAfterRead) {
  ValueCache<int> sut;
  (void)sut.getValue();
  EXPECT_EQ(false, sut.isNew());
}

TEST(ValueCacheTest, ValueSetsBackToNewAfterSet) {
  ValueCache<int> sut;
  (void)sut.getValue();
  sut.setValue(1);
  EXPECT_EQ(true, sut.isNew());
}

TEST(ValueCacheTest, ReadingYieldsCorrectValues) {
  ValueCache<int> sut;
  sut.setValue(1);
  EXPECT_EQ(1, sut.getValue());
  sut.setValue(10);
  EXPECT_EQ(10, sut.getValue());
}

TEST(ValueCacheTest, TryDifferentType) {
  ValueCache<std::string> sut;
  EXPECT_EQ(true, sut.isNew());
  EXPECT_EQ("", sut.getValue());

  (void)sut.getValue();
  EXPECT_EQ(false, sut.isNew());

  sut.setValue("abc");
  EXPECT_EQ(true, sut.isNew());

  EXPECT_EQ("abc", sut.getValue());
  sut.setValue("def");
  EXPECT_EQ("def", sut.getValue());
}
