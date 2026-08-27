/**
 * \file FDIR.hpp
 * \brief Consumes alarms from the health monitor and commands the rate damper.
 */

#pragma once
/// INCLUDES
#include <chrono>
#include <cstddef>

/// USER INCLUDES
#include "Alarm/Alarm.hpp"
#include "Alarm/AlarmReceiver.hpp"
#include "Databases/TelemetryDB.hpp"
#include "FDIR/AlarmQueue.hpp"
#include "FDIR/Configuration.hpp"

/// CODE

/**
 * \class FDIR
 * \note Dependency is one way HMS -> Alarm -> FDIR
 *  data is pulled straight from the telemetry. This module makes damping a
 * decision to an alarm event.

 */
class FDIR : public AlarmReceiver {
  static inline constexpr std::size_t alarmBufferSize{16};

public:
  FDIR(const fdirConfiguration &rhsConfiguration,
       const CriticalTelemetryDB &rhsDatabase) noexcept
      : faultConfiguration(rhsConfiguration), telemetryDB(rhsDatabase) {};

  // Runs in the HEALTH MONITOR's context, so it does one thing: enqueue.
  bool raiseAlarm(const alarmEntry &rhsInput) noexcept override;

  /**
   * \brief simulate the execution of a FDIR task being executed
   * general approach is parse all received alarms -> decide action
   */
  void tick(std::chrono::milliseconds rhsElapsedTime) noexcept;

private:
  /**
   * \brief Command the rate damper
   */
  double commandActuator() noexcept;

  /**
   * \brief filter the alarms to see if tehre is an action taken
   * no action taken for stale or cleared
   *
   * \param rhsEntry
   * \return true
   * \return false
   */
  bool isActionable(const alarmEntry &rhsEntry) noexcept;

  bool dampingRequested{false};
  bool actionableAlarm{true};
  std::size_t lastStepTime_ticks{0};

  fdirConfiguration faultConfiguration;
  const CriticalTelemetryDB &telemetryDB;
  AlarmQueue<alarmBufferSize> alarmBuffer{};
};
