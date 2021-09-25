#include "BandwidthData.h"
#include "BandwidthStatistics.h"
#include "gtest/gtest.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <string>
#include <memory>
#include <iostream>
 
const std::string testdata = "..//tests//testdata//";

std::unique_ptr<BandwidthData> bandwidthData;

TEST(BandwidthDataTest, NumberOfDays) {
  EXPECT_EQ(bandwidthData->GetNoOfDays(), 92);
}

TEST(BandwidthDataTest, DayDate) {
  BandwidthDay day = bandwidthData->GetDay("20210601");
  EXPECT_EQ(day.Date(), "20210601");
}

TEST(BandwidthDataTest, DayDataPointCount) {
  BandwidthDay day = bandwidthData->GetDay("20210601");
  const std::map<std::string, const std::unique_ptr<const BandwidthDataPoint>>& dataPoints = day.DataPoints();
  EXPECT_EQ(dataPoints.size(), 288);
}

TEST(BandwidthDataTest, AverageStatistic) {
  auto statistics = bandwidthData->GetStatistics("202106");
  if (statistics != NULL)
  {
    std::string requiredTime = "0000";
    double average = (*statistics)[requiredTime]->Average();
    double low = (*statistics)[requiredTime]->Low();
    double high = (*statistics)[requiredTime]->High();
    EXPECT_EQ(std::to_string(average), "55.459115");
    EXPECT_EQ(std::to_string(low), "7.649696");
    EXPECT_EQ(std::to_string(high), "67.493416");
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  bandwidthData = std::make_unique<BandwidthData>(testdata);
  std::this_thread::sleep_for(std::chrono::seconds(60));

  return RUN_ALL_TESTS();
}
