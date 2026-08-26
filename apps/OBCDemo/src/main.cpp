/**
 * \file OBC.cpp
 * \brief A brief description of what this file is.
 */

/// INCLUDES
// #include <csignal>
// #include <unistd.h>

/// INCLUDES
#include "HMS/Configuration.hpp"
#include <chrono>
#include <cstdint>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
#include "Telemetry/Telemetry.hpp"

/// NAMESPACE

/// DEFINES

/// GLOBAL VARIABLE SHARING FOR CURRENT SYSTEM TICK

/// CODE
int main() {

  // setup Telemetry to monitor
  using TelemetryIds::RateController1_x_AngularRate,
      TelemetryIds::Temperature_1, TelemetryIds::RateController2_x_AngularRate,
      TelemetryIds::Temperature_2, TelemetryIds::RateController3_x_AngularRate,
      TelemetryIds::Voltage, TelemetryIds::MAX;

  using milliseconds = std::chrono::milliseconds;

  constexpr healthMonitorConfiguration demoConfig{
      .sensorConfigs = {{
          {.name = "AngularRates",
           .units = "rad/sec",
           .samplingPeriod_ticks = 10,
           .lowLimit = .25,
           .highLimit = .25,
           .maxAbsoluteRateOfChange_UnitHz = 2.0,
           .telemetryToMonitor = {RateController1_x_AngularRate,
                                  RateController2_x_AngularRate,
                                  RateController3_x_AngularRate},
           .numberOfRollingAverageSamples = 3},
          {.name = "Temperatures",
           .units = "degC",
           .samplingPeriod_ticks = 1000,
           .lowLimit = .25,
           .highLimit = .25,
           .maxAbsoluteRateOfChange_UnitHz = 5.0,
           .telemetryToMonitor = {Temperature_1, Temperature_2, MAX},
           .numberOfRollingAverageSamples = 3},
          {.name = "Voltages",
           .units = "V",
           .samplingPeriod_ticks = 10,
           .lowLimit = .25,
           .highLimit = .25,
           .maxAbsoluteRateOfChange_UnitHz = 2.0,
           .telemetryToMonitor = {Voltage, MAX, MAX},
           .numberOfRollingAverageSamples = 3},
      }}};

  static_assert(demoConfig.isConfigurationRealistic(),
                "monitor configuration likely incorrect");

  milliseconds tickRate_ms_per_tick{1};

  milliseconds fault_1_msec{2000};
  milliseconds fault_2_msec{10000};

  milliseconds fault_3_msec{5000};

  while (true) {
    // sleep(10);
  }
  return 0;
}