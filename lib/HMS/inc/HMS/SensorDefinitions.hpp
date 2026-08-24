/**
 * \file Template.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
// #include <csignal>
// #include <cstdio>
#include <array>
#include <atomic>
#include <chrono>
#include <string>

/// CMAKE INCLUDES
// #include "Lib1/version.h"

/// USER INCLUDES
#include "HMS/TelemetryDB.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
struct sensorState {
  // Explicitly from question 2b
  // ensure these values are updated if we ever have an issue
  bool outOfRange_Low{false};
  bool outOfRange_High{false};
  bool outOfRange_RateOfChange{false};

  bool staleValues{false};

  double lastValue{0.0};
  double lastRateOfChange{0.0};

  std::size_t lastTMGenerationtime_ticks{};
  std::size_t numberOfSamples{0};
  bool hasfirstSample{false};
};

struct sensorConfiguration {
  std::string name{};
  std::string units{};

  std::size_t samplingPeriod_ticks{0};

  // LIMIT CONFIGURATIONS:
  // can be expanded
  // probably would add hysterisis
  double lowLimit{0.0};
  double highLimit{0.0};
  double maxAbsoluteRateOfChange_UnitHz{0.0};

  // placeholder
  std::size_t numberOfRollingAverageSamples{0};
};

struct healthMonitorConfiguration {

  std::array<sensorConfiguration, static_cast<std::size_t(TelemetryIds::MAX)>>
      sensorConfigs;
};