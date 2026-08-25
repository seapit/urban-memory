/**
 * \file Template.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <array>
#include <atomic>
#include <chrono>
#include <string_view>

/// CMAKE INCLUDES
// #include "Lib1/version.h"

/// USER INCLUDES
// #include "HMS/TelemetryDB.hpp"

/// NAMESPACE

/// DEFINES

/// CODE

/**
 * \brief sensorConfiguration
 * just a simple struct to hold a configuration
 * \note use stringview so this can be a constexpr at compile time
 * string_view.data() is nullterminated
 * ideally we configure our sensors/system on the ground and flesh it out fully
 * during design/dev
 */
struct sensorConfiguration {
  // For Logging/printouts
  std::string_view name{};
  std::string_view units{};

  std::size_t samplingPeriod_ticks{0};

  // LIMIT CONFIGURATIONS:
  double lowLimit{0.0};
  double highLimit{0.0};
  double maxAbsoluteRateOfChange_UnitHz{0.0};

  // placeholder
  std::size_t numberOfRollingAverageSamples{0};
};

/**
 * \brief healthMonitorConfiguration
 * A struct to instruct health monitoring how to monitor
 * \note isCOnfigurationRealistic exists to ensure that limits are set for the
 * indicated HMS configuration, the criteria for what is valid could change
 */
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