#include "BandwidthData.h"
#include "gtest/gtest.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
 
TEST(BandwidthDataTest, Negative) {
  BandwidthData a("..//tests//testdata//");
  std::this_thread::sleep_for(std::chrono::seconds(20));
  EXPECT_EQ(a.GetNoOfDays(), 92);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
