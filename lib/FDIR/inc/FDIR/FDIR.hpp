/**
 * \file FDIR.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <chrono>
#include <cstddef>

/// CMAKE INCLUDES
// #include "FDIR/version.h"

/// USER INCLUDES
#include "Alarm/Alarm.hpp"
#include "Alarm/AlarmReceiver.hpp"
#include "Common/HelperMacros.hpp"
#include "Databases/TelemetryDB.hpp"
#include "FDIR/AlarmQueue.hpp"
#include "FDIR/Configuration.hpp"
#include "Telemetry/Telemetry.hpp"
/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class FDIR
 */
class FDIR : public AlarmReceiver {
  static inline constexpr std::size_t alarmBufferSize{4};

public:
  /**
   * \brief Construct a new FDIR object
   *
   */
  FDIR(const fdirConfiguration &rhsConfiguration,
       const CriticalTelemetryDB &rhsDatabase)
      : faultConfiguration(rhsConfiguration), telemetryDB(rhsDatabase) {};

  bool raiseAlarm(const alarmEntry &rhsInput) noexcept override {
    return alarmBuffer.try_push(rhsInput);
  }

  // only one alarm can be actioned at a time, we are simulating an RTOS
  // tasks are bounded and shouldn't slip
  // decide which alarms are ok, THEN action the first one we can.
  void tick(std::chrono::milliseconds rhsElapsedTime) noexcept;

  void actionAlarm() {}
  // protected:

private:
  void checkAlarmValidity(const alarmEntry &rhsEntry) noexcept;

  bool isEligible(TelemetryIds lhsId) const noexcept;

  bool isValid(const alarmEntry &rhsEntry) const noexcept;
  bool actionableAlarm{false};
  [[maybe_unused]] bool actionedAlarm{false};
  [[maybe_unused]] bool continueActioning{false};

  fdirConfiguration faultConfiguration;
  [[maybe_unused]] const CriticalTelemetryDB &telemetryDB;
  AlarmQueue<16> alarmBuffer;
};
