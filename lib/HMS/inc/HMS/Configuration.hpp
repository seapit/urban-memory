/**
 * \file Template.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <array>
#include <atomic>
#include <chrono>
#include <string>

/// CMAKE INCLUDES
// #include "Lib1/version.h"

/// USER INCLUDES
// #include "HMS/TelemetryDB.hpp"

/// NAMESPACE

/// DEFINES

/// CODE

struct sensorConfiguration {
  // For Logging/printouts
  std::string name{};
  std::string units{};

  std::size_t samplingPeriod_ticks{0};

  // LIMIT CONFIGURATIONS:
  double lowLimit{0.0};
  double highLimit{0.0};
  double maxAbsoluteRateOfChange_UnitHz{0.0};

  // placeholder
  std::size_t numberOfRollingAverageSamples{0};
};

struct healthMonitorConfiguration {
  static inline constexpr std::size_t requiredNumberofChannels{3};

  std::array<sensorConfiguration, requiredNumberofChannels> sensorConfigs;

  // opportunity to show I know how to do single returns per function
  constexpr bool isConfigurationRealistic() const noexcept {
    bool isAValidConfig{true};
    for (const auto &configuration : sensorConfigs) {

      isAValidConfig = isAValidConfig &&
                       ((configuration.lowLimit <= configuration.highLimit) &&
                        (configuration.samplingPeriod_ticks != 0));
    }
    return isAValidConfig;
  }
};