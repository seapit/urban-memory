/**
 * \file TelemetryDefinitions.hpp
 * \brief hold the definitions for the different TM I want to use to set flags
 * for problem #2.b
 */

#pragma once
/// INCLUDES
// #include <csignal>
// #include <cstdio>
#include <cstdint>

/// CMAKE INCLUDES
// #include "Lib1/version.h"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES

/// CODE
struct TMBase {
  // this parameter must be updated to the last time we measured the sensor
  // value from the database
  std::size_t timeStamp{0};
  // Add the
  bool valid{false};

  // should use a rolling average as our rate of change error
  // brief doesn't indicate if instantaneous values should trigger it or if it's
  // sustained, should probably have a configuration for this
  // double rollingAverageOfDerivedObj{0.0};
  // Did the sensor update or not?
  // scope creep
  // bool sensorUpdateFault{false};

  // rate of change error
  bool rateChangeError{false};
  // thresholds
  bool maxThresholdExceeded{false};
  bool minThreshHoldExceeded{false};
};

struct DamperTM : public TMBase {
  double currentAngularRate{0.0};
  double commandedTorque{0.0};
  double averageValue{0.0};
};

// Go with range around 1.38V, it's the only value I genuinely remember
struct VoltageTM : public TMBase {
  double observedVoltage{0.0};
};

// Go with a range around 100
struct TemperatureSensorTM : public TMBase {
  double currentTemperature{0.0};
};
