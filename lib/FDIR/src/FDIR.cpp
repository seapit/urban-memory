/**
 * \file FDIR.cpp
 * \brief A brief description of what this file is.
 */

/// INCLUDES
// #include <csignal>
// #include <cstdio>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
#include "FDIR/FDIR.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
// FDIR::FDIR() { };

void FDIR::step(std::chrono::milliseconds rhsElapsedTime) noexcept {
  alarmEntry aAlarm;
  // loop will keep executing as long as there are things in the queue
  // we should ensure these are valid before we address them
  while (alarmBuffer.try_pop(aAlarm)) {
    checkAlarmValidity(aAlarm);
  }
}

void FDIR::checkAlarmValidity(const alarmEntry &rhsEntry) noexcept {
  if (isEligible(rhsEntry.id) == false) {
    return;
  }
  // a clear carries no cause bits by definition, so it cannot be gated on
  // the same predicate that recognises a fault
  if (rhsEntry.cause.cleared) {
    actionableAlarm = false;
  } else if (isValid(rhsEntry)) {
    actionableAlarm = true;
  }
}

bool FDIR::isEligible(TelemetryIds lhsId) const noexcept {
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

bool FDIR::isValid(const alarmEntry &rhsEntry) const noexcept {
  return (rhsEntry.cause.too_low || rhsEntry.cause.too_high ||
          rhsEntry.cause.rateofchange || rhsEntry.cause.stale);
}