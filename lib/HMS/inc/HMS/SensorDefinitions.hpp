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

/// NAMESPACE

/// DEFINES

/// CODE
struct tmSample {
  double readValue{0.0};
  std::size_t timestamp{0};
};

struct sensorState {
  bool outOfRange{false};
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

  std::chrono::milliseconds samplingPeriod_ms{};

  // can be expanded
  // probably would add hysterisis
  double lowLimit{0.0};
  double highLimit{0.0};

  // rate of change in units/sec
  double maxAbsoluteRateOfChange_UnitHz{0.0};

  // placeholder
  std::size_t numberOfRollingAverageSamples{0};
};

struct healthMonitorConfiguration {
  // from the brief, must monitor 3 different channels
  static inline constexpr std::size_t numberOfItemsMonitored{3};

  std::array<sensorConfiguration, numberOfItemsMonitored> sensorConfigs;
};