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
#include "Databases/TelemetryDB.hpp"
#include "HMS/Configuration.hpp"
#include "HMS/SensorState.hpp"
#include "Telemetry/Telemetry.hpp"

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
                const CriticalTelemetryDB &rhsDB)
      : stored_Configuration(rhsConfiguration), telemetryDatabase(rhsDB) {};

  // for the demo
  void step(std::chrono::milliseconds rhsElapsedTime) noexcept;

  /**
   * \brief indicate a sensor has received values, this is just to catch an
   * error on first use I saw at my job
   *
   * \param rhsId - id
   */
  void flagPrimedSensor(TelemetryIds rhsId) const noexcept;

  void evaluateRateOfChange() const noexcept;

  bool isOutOfLimits(TelemetryIds rhsID) const noexcept;

  // protected:

private:
  void executionLoop(std::size_t rhsChannel) const noexcept;

  healthMonitorConfiguration stored_Configuration;
  std::array<sensorState, healthMonitorConfiguration::requiredNumberofChannels>
      sensorStates;

  // sufficient for a rate of change calculation
  std::array<tmSample, healthMonitorConfiguration::requiredNumberofChannels>
      previousSample{};

  const CriticalTelemetryDB &telemetryDatabase;
};