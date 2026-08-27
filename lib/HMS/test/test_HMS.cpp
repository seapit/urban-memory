#include <chrono>
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <limits>

#include "Alarm/Alarm.hpp"
#include "Alarm/AlarmReceiver.hpp"
#include "Databases/TelemetryDB.hpp"
#include "HMS/Configuration.hpp"
#include "HMS/HealthMonitor.hpp"
#include "HMS/SensorState.hpp"
#include "Telemetry/Telemetry.hpp"

class aTestAlarmRcv : public AlarmReceiver {
public:
  bool raiseAlarm(const alarmEntry &rhs) noexcept override {

    bool aReturn{false};
    if ((rhs.cause & Alarm::rateofchange) != Alarm::empty) {
      OutOfRange_RoC = true;
      aReturn = true;
    }
    if ((rhs.cause & Alarm::too_high) != Alarm::empty) {
      OutOfRange_High = true;
      aReturn = true;
    }
    if ((rhs.cause & Alarm::too_low) != Alarm::empty) {
      OutOfRange_Low = true;
      aReturn = true;
    }
    if ((rhs.cause & Alarm::stale) != Alarm::empty) {
      OutOfRange_Stale = true;
      aReturn = true;
    }
    if (rhs.cause == Alarm::empty) {
      empty = true;
      aReturn = true;
    }

    return aReturn;
  }

  void reset() noexcept {
    OutOfRange_Stale = false;
    OutOfRange_Low = false;
    OutOfRange_High = false;
    OutOfRange_RoC = false;
    empty = false;
  }

  bool OutOfRange_Stale{false};
  bool OutOfRange_Low{false};
  bool OutOfRange_High{false};
  bool OutOfRange_RoC{false};
  bool empty{false};
};

// Define a test fixture class
class HMSTest : public ::testing::Test { // NOSONAR
protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  HMSTest() { // NOSONAR
    // You can do set-up work for each test here.
  }

  ~HMSTest() override { // NOSONAR
    // You can do clean-up work that doesn't throw exceptions here.
  }

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test).
    aTestSensor = {
        .outOfRange_Low = false,
        .outOfRange_High = false,
        .outOfRange_RateOfChange = false,

        .staleValues = false,
        .numberOfStale = 0,

        .lastValue = 0.0,
        .lastRateOfChange = 0.0,

        .lastTMGenerationtime_ticks = 0,
        .numberOfSamples = 0,
        .hasfirstSample = false,
    };

    aTestTelemetry = {0.0, 0};
    aAlarmRcver.reset();
    currentTick = 0;
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  void resetFlags(sensorState &rhsSensor) {
    rhsSensor.outOfRange_High = false;
    rhsSensor.outOfRange_Low = false;
    rhsSensor.outOfRange_RateOfChange = false;
  }

  void staleTripped() {
    EXPECT_EQ(aAlarmRcver.OutOfRange_Stale, true);
    EXPECT_EQ(aAlarmRcver.OutOfRange_Low, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_High, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_RoC, false);
    EXPECT_EQ(aAlarmRcver.empty, false);
  }
  void lowTripped() {
    EXPECT_EQ(aAlarmRcver.OutOfRange_Stale, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_Low, true);
    EXPECT_EQ(aAlarmRcver.OutOfRange_High, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_RoC, false);

    EXPECT_EQ(aAlarmRcver.empty, false);
  }
  void highTripped() {
    EXPECT_EQ(aAlarmRcver.OutOfRange_Stale, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_Low, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_High, true);
    EXPECT_EQ(aAlarmRcver.OutOfRange_RoC, false);

    EXPECT_EQ(aAlarmRcver.empty, false);
  }
  void rocTripped() {
    EXPECT_EQ(aAlarmRcver.OutOfRange_Stale, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_Low, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_High, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_RoC, true);
    EXPECT_EQ(aAlarmRcver.empty, false);
  }
  void noneTripped() {
    EXPECT_EQ(aAlarmRcver.OutOfRange_Stale, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_Low, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_High, false);
    EXPECT_EQ(aAlarmRcver.OutOfRange_RoC, false);

    EXPECT_EQ(aAlarmRcver.empty, false);
  }

  void tick(bool forcePublish, double rhsValue) {
    // update current tick
    currentTick += ticks;

    const TelemetryIds aId = aTestConfig.sensorConfigs[1].telemetryToMonitor[0];
    // if we need a tm
    if (forcePublish)
      aTestDb.publish(aId, {rhsValue, currentTick});

    // call the HMS val
    aMonitor.checkMonitorCondition(std::chrono::milliseconds(currentTick));
  };

  double getRateAbidingValue(double prev) {

    // ROC = (X2 - X1) / ((t2 -1t)/1000) = 1000*(x2-x1)/(10)
    // solve for x2

    // RoC/100 = x2 -x1
    // RoC/100 + x1 = x2

    return (
        (aTestConfig.sensorConfigs[1].maxAbsoluteRateOfChange_UnitHz / 100) +
        prev);
  }

  // use the same configuration from main I guess
  healthMonitorConfiguration aTestConfig{
      // use middle to ensure we skip a config
      .sensorConfigs = {{{},
                         {
                             .name = "AngularRates",
                             .units = "rad/sec",
                             .samplingPeriod_ticks = 10,
                             .lowLimit = .25,
                             .highLimit = .5,
                             .maxAbsoluteRateOfChange_UnitHz = 2.0,
                             // just need 1 tm to test this all
                             .telemetryToMonitor =
                                 {TelemetryIds::RateController1_x_AngularRate,
                                  TelemetryIds::MAX, TelemetryIds::MAX},
                             .numberPermittedStaleUpdates = 3,
                             .numberOfRollingAverageSamples = 0,
                         },
                         {}}}};

  static constexpr std::size_t ticks{10};
  std::size_t currentTick{0};

  CriticalTelemetryDB aTestDb;
  aTestAlarmRcv aAlarmRcver;
  sensorState aTestSensor;
  tmSample aTestTelemetry;
  HealthMonitor aMonitor{aTestConfig, aTestDb, aAlarmRcver};
};

// Test cases using the test fixture
// use this as a linker test normally
TEST_F(HMSTest, testPrimed) {

  aTestSensor.numberOfSamples = 10;

  aTestTelemetry = {.readValue = 1.0, .timestamp = 1};

  EXPECT_NO_FATAL_FAILURE(
      aMonitor.flagPrimedSensor(aTestSensor, aTestTelemetry));

  // ensure unchanged
  EXPECT_EQ(aTestSensor.outOfRange_Low, false);
  EXPECT_EQ(aTestSensor.outOfRange_High, false);
  EXPECT_EQ(aTestSensor.outOfRange_RateOfChange, false);

  EXPECT_EQ(aTestSensor.staleValues, false);
  EXPECT_EQ(aTestSensor.numberOfStale, 0);

  EXPECT_EQ(aTestSensor.lastRateOfChange, 0.0);

  EXPECT_EQ(aTestSensor.numberOfSamples, 10);

  // test changed
  EXPECT_EQ(aTestSensor.lastValue, 1.0);
  EXPECT_EQ(aTestSensor.lastTMGenerationtime_ticks, 1);
  EXPECT_EQ(aTestSensor.hasfirstSample, true);
}

TEST_F(HMSTest, testROCEvaluation) {

  aTestTelemetry = {.readValue = 10.0, .timestamp = 1000};

  // elapsedSeconds = (1000 - 0)/ 1000 = 1000/1000=1

  // expect RATE OF CHANGE = 10.0
  EXPECT_NO_FATAL_FAILURE(
      aMonitor.evaluateRateOfChange(aTestSensor, aTestTelemetry));

  // ensure unchanged
  EXPECT_EQ(aTestSensor.outOfRange_Low, false);
  EXPECT_EQ(aTestSensor.outOfRange_High, false);
  EXPECT_EQ(aTestSensor.outOfRange_RateOfChange, false);

  EXPECT_EQ(aTestSensor.staleValues, false);
  EXPECT_EQ(aTestSensor.numberOfStale, 0);

  EXPECT_EQ(aTestSensor.lastValue, 0.0);
  EXPECT_EQ(aTestSensor.lastTMGenerationtime_ticks, 0);
  EXPECT_EQ(aTestSensor.numberOfSamples, 0);
  EXPECT_EQ(aTestSensor.hasfirstSample, false);

  // test changed
  // 10-0/1 = 10
  EXPECT_EQ(aTestSensor.lastRateOfChange, 10.0);
}

TEST_F(HMSTest, testLimitReturn) {
  // initialized to have all things = 0
  bool aReturnValue{false};

  EXPECT_NO_FATAL_FAILURE(aReturnValue = aMonitor.isOutOfLimits(aTestSensor));
  EXPECT_EQ(aReturnValue, false);

  aTestSensor.outOfRange_High = true;
  EXPECT_NO_FATAL_FAILURE(aReturnValue = aMonitor.isOutOfLimits(aTestSensor));
  EXPECT_EQ(aReturnValue, true);
  aReturnValue = false;
  resetFlags(aTestSensor);

  aTestSensor.outOfRange_Low = true;
  EXPECT_NO_FATAL_FAILURE(aReturnValue = aMonitor.isOutOfLimits(aTestSensor));
  EXPECT_EQ(aReturnValue, true);
  aReturnValue = false;
  resetFlags(aTestSensor);

  aTestSensor.outOfRange_RateOfChange = true;
  EXPECT_NO_FATAL_FAILURE(aReturnValue = aMonitor.isOutOfLimits(aTestSensor));
  EXPECT_EQ(aReturnValue, true);
  aReturnValue = false;
  resetFlags(aTestSensor);

  aTestSensor.outOfRange_High = true;
  aTestSensor.outOfRange_Low = true;
  aTestSensor.outOfRange_RateOfChange = true;
  EXPECT_NO_FATAL_FAILURE(aReturnValue = aMonitor.isOutOfLimits(aTestSensor));
  EXPECT_EQ(aReturnValue, true);
  aReturnValue = false;
  resetFlags(aTestSensor);
}

TEST_F(HMSTest, testExecLoop) {
  EXPECT_EQ(aTestConfig.isConfigurationRealistic(), true);

  auto lowValue = aTestConfig.sensorConfigs[1].lowLimit;
  auto highValue = aTestConfig.sensorConfigs[1].highLimit;
  auto intermediateValue = (highValue + lowValue) / 2.0;

  // test a non-alarm TM
  EXPECT_NO_FATAL_FAILURE(tick(true, lowValue));
  std::cout << "low valid value" << std::endl;
  noneTripped();
  EXPECT_NO_FATAL_FAILURE(tick(true, intermediateValue));
  std::cout << "intermediate value" << std::endl;
  noneTripped();
  EXPECT_NO_FATAL_FAILURE(tick(true, highValue));
  std::cout << "high valid value" << std::endl;
  noneTripped();

  // test low value
  std::cout << "low invalid value" << std::endl;
  EXPECT_NO_FATAL_FAILURE(
      // nextafter (I discovered writing this unit test), allows you to reduce a
      // double by exactly 1 bit. helping for testing this and being a
      // stickler
      tick(true,
           std::nextafter(lowValue, -std::numeric_limits<double>::infinity())));
  lowTripped();
  // clear so we can test the rest
  aAlarmRcver.reset();

  // will need to clear for RoC
  std::cout << "prevent RoC Error" << std::endl;
  EXPECT_NO_FATAL_FAILURE(tick(true, intermediateValue));
  aAlarmRcver.reset();

  // test high value
  std::cout << "high invalid value" << std::endl;
  EXPECT_NO_FATAL_FAILURE(
      tick(true,
           std::nextafter(highValue, std::numeric_limits<double>::infinity())));
  highTripped();
  // clear so we can test the rest
  aAlarmRcver.reset();

  // elapsedSeconds = (1000 - 0)/ 1000 = 1000/1000=1

  // expect RATE OF CHANGE = 10.0
  auto aROCValue = std::nextafter(
      aTestConfig.sensorConfigs[1].maxAbsoluteRateOfChange_UnitHz,
      std::numeric_limits<double>::infinity());

  // ROC = (X2 - X1) / ((t2 -1t)/1000) = 1000*(x2-x1)/(10)
  // ROC * (t2-t1)/1000 = x2 -x1
  // RoC / 100  = x2- x1
  // ROC /100 + x1 = x2
  auto aValue = (aROCValue / 100) + highValue;

  std::cout << "invalid roc" << std::endl;
  EXPECT_NO_FATAL_FAILURE(tick(true, aValue));
  rocTripped();
  aAlarmRcver.reset();

  // test stale
  std::cout << "stale pass 1" << std::endl;
  EXPECT_NO_FATAL_FAILURE(tick(false, intermediateValue));
  noneTripped();
  std::cout << "stale pass 2" << std::endl;
  EXPECT_NO_FATAL_FAILURE(tick(false, intermediateValue));
  noneTripped();
  std::cout << "stale pass 3" << std::endl;
  EXPECT_NO_FATAL_FAILURE(tick(false, intermediateValue));
  staleTripped();
}