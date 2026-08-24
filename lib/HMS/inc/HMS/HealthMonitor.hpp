/**
 * \file HealthMonitor.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <array>

/// CMAKE INCLUDES
// #include "HMS/version.h"

/// USER INCLUDES
#include "HMS/Configuration.hpp"
#include "HMS/SensorState.hpp"
#include "HMS/TelemetryDB.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class HealthMonitor
 */
class HealthMonitor {
public:
  /**
   * \brief Construct a new HealthMonitor object
   *
   */
  HealthMonitor(healthMonitorConfiguration &rhsConfiguration,
                const TelemetryDB &rhsDB)
      : stored_Configuration(rhsConfiguration), telemetryDatabase(rhsDB) {};

  void step(std::chrono::milliseconds rhsElapsedTime) noexcept;

  void flagPrimedSensor(TelemetryIds rhsId) const noexcept;

  void evaluateRateOfChange() const noexcept;

  // protected:

private:
  healthMonitorConfiguration stored_Configuration;
  std::array<sensorState, healthMonitorConfiguration::requiredNumberofChannels>
      sensorStates;

  // sufficient for a rate of change calculation
  std::array<tmSample, healthMonitorConfiguration::requiredNumberofChannels>
      previousSample{};

  const TelemetryDB &telemetryDatabase;
};