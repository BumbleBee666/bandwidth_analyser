#include "BandwidthData.h"
#include "gtest/gtest.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <string>
#include <memory>
 
const std::string testdata = "..//tests//testdata//";

std::unique_ptr<BandwidthData> bandwidthData;

TEST(BandwidthDataTest, NumberOfDays) {
  EXPECT_EQ(bandwidthData->GetNoOfDays(), 92);
}

TEST(BandwidthDataTest, DayDate) {
  BandwidthDay day = bandwidthData->GetDay("20210601");
  EXPECT_EQ(day.Date(), "20210601");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  bandwidthData = std::make_unique<BandwidthData>(testdata);
  std::this_thread::sleep_for(std::chrono::seconds(30));

  return RUN_ALL_TESTS();
}
