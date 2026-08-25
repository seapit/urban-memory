/**
 * \file HealthMonitor.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <array>
#include <cstddef>

/// CMAKE INCLUDES
// #include "HMS/version.h"

/// USER INCLUDES
#include "Alarm/AlarmReceiver.hpp"
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
  HealthMonitor(const healthMonitorConfiguration &rhsConfiguration,
                const CriticalTelemetryDB &rhsDB, AlarmReceiver &rhsAlarmer)
      : stored_Configuration(rhsConfiguration), telemetryDatabase(rhsDB),
        alarmDestination(rhsAlarmer) {};

  // for the demo
  void step(std::chrono::milliseconds rhsElapsedTime) noexcept;

  /**
   * \brief indicate a sensor has received values, this is just to catch an
   * error on first use I saw at my job
   *
   * \param rhsId - id
   */
  void flagPrimedSensor(sensorState &rhsSensor,
                        const tmSample &rhsSample) const noexcept;

  void evaluateRateOfChange(sensorState &rhsSensor,
                            const tmSample &rhsSample) const noexcept;

  bool isOutOfLimits(const sensorState &rhsSensor) const noexcept;

  // protected:

private:
  void executionLoop(const std::size_t rhsIndex) noexcept;

  healthMonitorConfiguration stored_Configuration;
  std::array<sensorState, sizecast(TelemetryIds::MAX)> sensorStates;

  // sufficient for a rate of change calculation
  std::array<tmSample, healthMonitorConfiguration::requiredNumberofChannels>
      previousSample{};

  const CriticalTelemetryDB &telemetryDatabase;
  AlarmReceiver &alarmDestination;
};