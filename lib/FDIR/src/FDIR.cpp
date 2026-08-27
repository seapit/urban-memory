/**
 * \file FDIR.cpp
 * \brief A brief description of what this file is.
 */

/// INCLUDES
// #include <csignal>
// #include <cstdio>
#include <iostream>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
#include "Common/HelperMacros.hpp"
#include "FDIR/FDIR.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
// Runs within whoever's called it
bool FDIR::raiseAlarm(const alarmEntry &rhsInput) noexcept {
  return alarmBuffer.try_push(rhsInput);
}

// logic to advance in time
void FDIR::tick(std::chrono::milliseconds rhsElapsedTime) noexcept {
  const auto ticks = static_cast<std::size_t>(rhsElapsedTime.count());

  if (ticks == 0) {
    return;
  }

  lastStepTime_ticks = sizecast(rhsElapsedTime.count());

  // create a receiver for the alarms in the buffer
  alarmEntry aAlarm{};
  // get alarm from out of queue
  while (alarmBuffer.try_pop(aAlarm)) {
    isActionable(aAlarm);
  }
  std::cout << "FDIR commanded torque of: " << commandActuator() << std::endl;
}

double FDIR::commandActuator() noexcept {
  aRetVal{0.0};

  const configuredAction &telemetry =
      faultConfiguration.actionableTelemetries[0];

  // if command is configured for an action, and there is something to do
  if (telemetry.isConfigured() && dampingRequested && actionableAlarm) {

    // pull tm from the database
    const tmSample aLatest = telemetryDB.getLatest(telemetry.id);

    aReturn = telemetry.damper->control(aLatest.readValue);
  }

  return aReturn;
}

bool FDIR::isActionable(const alarmEntry &rhsEntry) noexcept {

  // if we're stale we should just skip
  if (rhsEntry.cause.stale == 1) {
    actionableAlarm = false;
    return false;
  }

  // if alarm was just cleared,
  // there is nothing to action
  if (rhsEntry.cause.cleared) {
    dampingRequested = false;
    actionableAlarm = true;
    return false;
  }

  // if any of the established failure criteria
  if ((rhsEntry.cause.too_low == 1) || (rhsEntry.cause.too_high == 1) ||
      (rhsEntry.cause.rateofchange == 1)) {
    dampingRequested = true;
    actionableAlarm = true;
  }
  return true;
}