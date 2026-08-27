/**
 * \file Template.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include "Telemetry/Telemetry.hpp"
#include <array>
#include <cstddef>
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
 * during design/dev.
 * \note telemetries that have the same error criteria should be placed in the
 * same channel configuration
 * \todo ensure that the demo generates 6 Tm (3/2/1 for each channel)
 */
struct channelConfiguration {
  static constexpr std::size_t maximumNumberOfIDs{3};

  // helper function to initialize the elements of an array
  static constexpr std::array<TelemetryIds, maximumNumberOfIDs>
  fillArrayWithInvalid() noexcept {
    std::array<TelemetryIds, maximumNumberOfIDs> aTemporaryArray;

    for (auto &element : aTemporaryArray) {
      element = TelemetryIds::MAX;
    }
    return aTemporaryArray;
  }

  // For Logging/printouts
  std::string_view name{};
  std::string_view units{};

  std::size_t samplingPeriod_ticks{0};

  // LIMIT CONFIGURATIONS:
  double lowLimit{0.0};
  double highLimit{0.0};
  double maxAbsoluteRateOfChange_UnitHz{0.0};

  std::array<TelemetryIds, maximumNumberOfIDs> telemetryToMonitor =
      fillArrayWithInvalid();

  std::size_t numberPermittedStaleUpdates{0};

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

  std::array<channelConfiguration, requiredNumberofChannels> sensorConfigs;

  // opportunity to show I know how to do single returns per function
  constexpr bool isConfigurationRealistic() const noexcept {
    bool isAValidConfig{true};
    for (const auto &configuration : sensorConfigs) {

      // skip disabled/unconfigured channelconfigs
      if (configuration.samplingPeriod_ticks == 0)
        continue;

      isAValidConfig = isAValidConfig &&
                       ((configuration.lowLimit <= configuration.highLimit) &&
                        (configuration.samplingPeriod_ticks != 0));
    }
    return isAValidConfig;
  }
};