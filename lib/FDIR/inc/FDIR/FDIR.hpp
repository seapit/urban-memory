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
  void
  step([[maybe_unused]] std::chrono::milliseconds rhsElapsedTime) noexcept {
    alarmEntry aAlarm;
    // loop will keep executing as long as there are things in the queue
    // we should ensure these are valid before we address them
    while (alarmBuffer.try_pop(aAlarm)) {
      checkAlarmValidity(aAlarm);
    }
  }

  void actionAlarm() {}
  // protected:

private:
  void checkAlarmValidity(const alarmEntry &rhsEntry) {

    // filter out TMs we cant action
    // ensure it's valid
    if (isValid(rhsEntry) && isEligible(rhsEntry.id)) {
      actionableAlarm = true;
    }
    // check if we've corrected
    if (isValid(rhsEntry) && (rhsEntry.cause == Alarm::empty)) {
      actionableAlarm = false;
    }
  }

  bool isEligible(TelemetryIds lhsId) {
    aRetVal{false};
    for (const auto &[workableId, _] : faultConfiguration.eligibleTelemetries) {
      if (lhsId == workableId) {
        {
          aReturn = true;
          break;
        }
      }
    }
    return aReturn;
  }

  bool isValid(const alarmEntry &rhsEntry) {
    bool aReturn = false;

    if (static_cast<bool>(rhsEntry.cause & Alarm::rateofchange) ||
        static_cast<bool>(rhsEntry.cause & Alarm::too_high) ||
        static_cast<bool>(rhsEntry.cause & Alarm::too_low)) {
      aReturn = true;
    }
    return aReturn;
  }

  [[maybe_unused]] bool actionableAlarm{false};
  [[maybe_unused]] bool actionedAlarm{false};
  [[maybe_unused]] bool continueActioning{false};

  fdirConfiguration faultConfiguration;
  [[maybe_unused]] const CriticalTelemetryDB &telemetryDB;
  AlarmQueue<16> alarmBuffer;
};
