/**
 * \file HMS.cpp
 * \brief A brief description of what this file is.
 */

/// INCLUDES
// #include <csignal>
// #include <cstdio>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
#include "HMS/HealthMonitor.hpp"
#include "Alarm/Alarm.hpp"
#include "Common/HelperMacros.hpp"
#include "HMS/Configuration.hpp"
#include "Telemetry/Telemetry.hpp"

#include <cstddef>

/// NAMESPACE

/// DEFINES

/// CODE
void HealthMonitor::checkMonitorCondition(
    std::chrono::milliseconds rhsElapsedTime) noexcept {
  const auto ticks = static_cast<std::size_t>(rhsElapsedTime.count());

  if (ticks == 0) {
    return;
  }

  const std::size_t elapsedTicks = sizecast(rhsElapsedTime.count());

  for (std::size_t i = 0;
       i < sizecast(healthMonitorConfiguration::requiredNumberofChannels);
       ++i) {

    const std::size_t samplingPeriod =
        stored_Configuration.sensorConfigs[i].samplingPeriod_ticks;

    // if cancelled or polling period not met
    if ((samplingPeriod == 0) || ((elapsedTicks % samplingPeriod) != 0)) {
      continue;
    }
    validateChannel(i);
  }
}

void HealthMonitor::flagPrimedSensor(sensorState &rhsSensor,
                                     const tmSample &rhsSample) const noexcept {

  rhsSensor.hasfirstSample = true;
  rhsSensor.lastValue = rhsSample.readValue;
  rhsSensor.lastTMGenerationtime_ticks = rhsSample.timestamp;
}

void HealthMonitor::evaluateRateOfChange(
    sensorState &rhsSensor, const tmSample &rhsSample) const noexcept {

  const double elapsedSeconds =
      // this is in ticks, 1 tick ~ 1ms
      (rhsSample.timestamp - rhsSensor.lastTMGenerationtime_ticks) / 1000.0;

  // prevent division by 0
  if (elapsedSeconds > 0.0) {
    // get rate of change
    rhsSensor.lastRateOfChange =
        (rhsSample.readValue - rhsSensor.lastValue) / elapsedSeconds;
  }
}

bool HealthMonitor::isOutOfLimits(const sensorState &rhsSensor) const noexcept {
  return (rhsSensor.outOfRange_High || rhsSensor.outOfRange_Low ||
          rhsSensor.outOfRange_RateOfChange);
}

void HealthMonitor::validateChannel(const std::size_t rhsIndex) noexcept {
  // get the configuration we need for this channel's index
  const auto &aChannelCfg = stored_Configuration.sensorConfigs[rhsIndex];

  // for each channel's associated telemetry
  for (const auto &associatedTM : aChannelCfg.telemetryToMonitor) {

    // if the associated telemetry is not configured for this channel
    if (associatedTM == TelemetryIds::MAX) {
      continue;
    }

    // get the appropriate sensor
    auto &aSensor = sensorStates[sizecast(associatedTM)];

    // load its telemetry
    const auto &aTelemetry = telemetryDatabase.getLatest(associatedTM);

    // prevent triggering on the first value
    if (aSensor.hasfirstSample == false) {
      //  telemetry.timeStamp is non-zero 0 we are receiving data
      if (aTelemetry.timestamp != 0) {
        flagPrimedSensor(aSensor, aTelemetry);
      }
      // ensure we still loop through the others to make sure they've received
      // TM the first TM sample shouldn't trigger FDIR
      continue;
    }

    // a stale sensor is one that has missed a deadline to update
    // so aTelemetry.timestamp = aSensor.lastTMGenerationTime_ticks
    if (aTelemetry.timestamp == aSensor.lastTMGenerationtime_ticks) {
      aSensor.numberOfStale += 1;

      // if we have detected a stale sensor
      // flag on the rising edge
      if ((aSensor.numberOfStale >= aChannelCfg.numberPermittedStaleUpdates) &&
          (aSensor.staleValues == false)) {
        // set sensor to have stale values
        aSensor.staleValues = true;

        // the sensor has stale values so any of the other alarms criteria may
        // not apply anymore
        aSensor.outOfRange_High = false;
        aSensor.outOfRange_Low = false;
        aSensor.outOfRange_RateOfChange = false;

        // create alarm
        alarmEntry alarmToRaise{
            .id = associatedTM, .cause = {.stale = 1}, .sample = aTelemetry};
        // raise it
        // i dont have logic to raise this error if the raising of alarm doesn't
        // work
        (void)alarmDestination.raiseAlarm(alarmToRaise);
      }
      continue;
    }

    // if we got here we have not executed the stale 'continue'
    aSensor.numberOfStale = 0;
    // reset stale flag and alarm
    if (aSensor.staleValues) {
      aSensor.staleValues = false;
      alarmEntry alarmToRaise{
          .id = associatedTM, .cause = {.cleared = 1}, .sample = aTelemetry};
      (void)alarmDestination.raiseAlarm(alarmToRaise);
    }

    // get elapsed time
    evaluateRateOfChange(aSensor, aTelemetry);

    // false = was never out of limits
    auto wasOutOfLimits = isOutOfLimits(aSensor);

    // Update the monitored flags
    aSensor.outOfRange_High = (aTelemetry.readValue > aChannelCfg.highLimit);
    aSensor.outOfRange_Low = (aTelemetry.readValue < aChannelCfg.lowLimit);

    // ensure we don't trip on this not being configured
    aSensor.outOfRange_RateOfChange =
        (aChannelCfg.maxAbsoluteRateOfChange_UnitHz > 0.0) &&
        (std::abs(aSensor.lastRateOfChange) >
         aChannelCfg.maxAbsoluteRateOfChange_UnitHz);

    const auto currentlyOutOfBounds = isOutOfLimits(aSensor);

    // convenience lambdas
    auto alarmState = [&aSensor] {
      aRetVal = Alarm{};
      if (aSensor.outOfRange_High)
        aReturn.too_high = 1;
      if (aSensor.outOfRange_Low)
        aReturn.too_low = 1;
      if (aSensor.outOfRange_RateOfChange)
        aReturn.rateofchange = 1;
      return aReturn;
    };

    // trigger on rising edge
    if ((wasOutOfLimits == false) && (currentlyOutOfBounds == true)) {
      alarmEntry aAlarmEntry{
          .id = associatedTM, .cause = alarmState(), .sample = aTelemetry};
      alarmDestination.raiseAlarm(aAlarmEntry);
    }
    // clear if we recovered
    else if ((wasOutOfLimits == true) && currentlyOutOfBounds == false) {
      alarmEntry aAlarmEntry{
          .id = associatedTM, .cause = {.cleared = 1}, .sample = aTelemetry};
      alarmDestination.raiseAlarm(aAlarmEntry);
    }
    // update last values so we can get the rate of change next time
    aSensor.lastValue = aTelemetry.readValue;
    aSensor.lastTMGenerationtime_ticks = aTelemetry.timestamp;
  }
}
