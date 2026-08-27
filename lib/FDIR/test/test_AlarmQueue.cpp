#include <gtest/gtest.h>
#include <memory>

#include "Alarm/Alarm.hpp"
#include "Alarm/AlarmReceiver.hpp"
#include "FDIR/AlarmQueue.hpp"

// Define a test fixture class
class AlarmQueueTest : public ::testing::Test { // NOSONAR
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  AlarmQueueTest() { // NOSONAR
    // You can do set-up work for each test here.
  }

  ~AlarmQueueTest() override { // NOSONAR
    // You can do clean-up work that doesn't throw exceptions here.
  }

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test).
    aTestBuf = std::make_unique<AlarmQueue<queueSize>>();
  }

  std::string_view getString(Alarm rhs) {
    std::string_view aReturnStringView{};

    switch (rhs) {
    case (rhs.too_low):
      aReturnStringView = " too_low";
      break;
    case (rhs.too_high):
      aReturnStringView = " too_high ";
      break;

    case Alarm::rateofchange:
      aReturnStringView = " rateofchange ";
      break;
    case Alarm::stale:
      aReturnStringView = " stale";
      break;
    default:
      break;
    }
    return aReturnStringView;
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
  static constexpr std::size_t queueSize{4};
  std::unique_ptr<AlarmQueue<queueSize>> aTestBuf;
};

// Test cases using the test fixture
TEST_F(AlarmQueueTest, InitProbably) {}

TEST_F(AlarmQueueTest, pushAlarm) {
  using Alarm::too_high, Alarm::rateofchange, Alarm::too_low, Alarm::stale;
  using TelemetryIds::RateController1_x_AngularRate,
      TelemetryIds::Temperature_1, TelemetryIds::RateController2_x_AngularRate,
      TelemetryIds::Temperature_2, TelemetryIds::RateController3_x_AngularRate,
      TelemetryIds::Voltage, TelemetryIds::MAX;

  Alarm aAlarm = too_low;
  bool toValidate{false};

  alarmEntry aAlarmEntry{.id = RateController1_x_AngularRate,
                         .cause = aAlarm,
                         .sample{.readValue = 0, .timestamp = 0}};

  for (std::size_t i = 0; i < queueSize + 4; ++i) {
    // eexpect push to succeed/not crash
    std::cout << getString(aAlarm) << std::endl;
    EXPECT_NO_FATAL_FAILURE(toValidate = aTestBuf->try_push(aAlarmEntry));
    if (i < queueSize)
      EXPECT_EQ(toValidate, true);
    else {
      EXPECT_EQ(toValidate, false);
    }

    // update alarm cause for next run
    switch (i) {
    case 0:
      aAlarm = too_high;
      break;
    case 1:
      aAlarm = rateofchange;
      break;
    case 2:
      aAlarm = stale;
      break;
    case 3:
      aAlarm = too_low;
      break;
    case 4:
      aAlarm = too_high;
      break;
    default:
      break;
    }

    // set the Alarm Entry
    aAlarmEntry.cause = aAlarm;
  }

  std::cout << std::endl;
  std::cout << std::endl;
  aAlarm = too_low;

  EXPECT_EQ(aTestBuf->getEntries(), queueSize);
  EXPECT_EQ(aTestBuf->getOverFlows(), 4);

  for (std::size_t i = 0; i <= queueSize + 4; ++i) {
    // eexpect push to succeed/not crash
    // std::cout << getString(aAlarm) << std::endl;

    EXPECT_NO_FATAL_FAILURE(toValidate = aTestBuf->try_pop(aAlarmEntry));

    if (i < queueSize)
      EXPECT_EQ(toValidate, true);
    else
      EXPECT_EQ(toValidate, false);

    EXPECT_EQ(aAlarmEntry.cause, aAlarm);

    switch (i) {
    case 0:
      aAlarm = too_high;
      break;
    case 1:
      aAlarm = rateofchange;
      break;
    case 2:
      aAlarm = stale;
      break;
    case 3:
      aAlarm = too_low;
      break;
    default:
      break;
    }

    // set the Alarm Entry
    aAlarmEntry.cause = aAlarm;
  }
}
