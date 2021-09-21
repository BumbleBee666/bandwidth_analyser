#include "BandwidthData.h"
#include "gtest/gtest.h"

// Tests factorial of negative numbers.
TEST(BandwidthDataTest, Negative) {
  BandwidthData a("/bandwidth_analyser/");
  EXPECT_GT(a.GetNoOfDays(), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
