// #include "Register.h"
#include <gtest/gtest.h>
// #include "Nextion.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
        ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}


TEST(OsalTests, TestInit) {
  ASSERT_TRUE(true);
}