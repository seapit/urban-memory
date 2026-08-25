/**
 * \file FDIR.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <chrono>
#include <cstdint>

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

  void raiseAlarm(const alarmEntry &rhsInput) noexcept override {
    alarmBuffer.try_push(rhsInput);
  }

  void step(std::chrono::milliseconds rhsElapsedTime) noexcept {
    alarmEntry aAlarm;
    // loop will keep executing as long as there are things in the queue
    while (alarmBuffer.try_pop(aAlarm)) {
      actionAlarm(aAlarm);
    }
  }

  // protected:

private:
  void actionAlarm(const alarmEntry &rhsEntry) {

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
    aRetVal{true};
    for (const auto &[workableId, _] : faultConfiguration.eligibleTelemetries) {
      if (lhsId == workableId) {
        {
          aReturn = true;
          break;
        }
      }
      return aReturn;
    }
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

  bool actionableAlarm{false};
  bool actionedAlarm{false};
  bool continueActioning{false};

  fdirConfiguration faultConfiguration;
  const CriticalTelemetryDB &telemetryDB;
  AlarmQueue<16> alarmBuffer;
};
