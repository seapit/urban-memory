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
#include <cstdlib>

/// NAMESPACE

/// DEFINES

/// CODE
void HealthMonitor::step(std::chrono::milliseconds rhsElapsedTime) noexcept {
  const auto ticks = static_cast<std::size_t>(rhsElapsedTime.count());

  if (ticks == 0) {
    return;
  }

  for (std::size_t i = 0;
       i < sizecast(healthMonitorConfiguration::requiredNumberofChannels);
       ++i) {
    executionLoop(i);
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

void HealthMonitor::executionLoop(const std::size_t rhsIndex) const noexcept {
  // get the configuration we need for this channel's index
  auto aChannelCfg = stored_Configuration.sensorConfigs[rhsIndex];

  // for each channel's associated telemetry
  for (const auto &associatedTM : aChannelCfg.telemetryToMonitor) {
    // get the appropriate sensor
    auto aSensor = sensorStates[sizecast(associatedTM)];

    // load its telemetry
    const auto &aTelemetry = telemetryDatabase.getLatest(associatedTM);

    // prevent triggering on the first value
    if (aSensor.hasfirstSample == false) {
      flagPrimedSensor(aSensor, aTelemetry);
      // ensure we still loop through the others to make sure they've received
      // TM the first TM sample shouldn't trigger FDIR
      continue;
    }

    // get elapsed time
    evaluateRateOfChange(aSensor, aTelemetry);

    // convenience lambdas
    auto outOfBounds = [&aSensor] {
      return (aSensor.outOfRange_High || aSensor.outOfRange_Low ||
              aSensor.outOfRange_RateOfChange);
    };

    auto alarmState = [&aSensor] {
      aRetVal{Alarm::empty};
      if (aSensor.outOfRange_High)
        aReturn |= Alarm::too_high;
      if (aSensor.outOfRange_Low)
        aReturn |= Alarm::too_low;
      if (aSensor.outOfRange_RateOfChange)
        aReturn |= Alarm::rateofchange;
      return aReturn;
    };

    // false = was never out of limits
    auto aPreviousState = isOutOfLimits(aSensor);

    // Update the monitored flags
    aSensor.outOfRange_High = (aTelemetry.readValue > aChannelCfg.highLimit);
    aSensor.outOfRange_Low = (aTelemetry.readValue < aChannelCfg.lowLimit);

    // ensure we don't trip on this not being configured
    aSensor.outOfRange_RateOfChange =
        (aChannelCfg.maxAbsoluteRateOfChange_UnitHz > 0.0) &&
        (std::abs(aSensor.lastRateOfChange) >
         aChannelCfg.maxAbsoluteRateOfChange_UnitHz);

    const auto aCurrentFlag = outOfBounds();

    // trigger on rising edge
    if ((aPreviousState == false) && (aCurrentFlag == true)) {
      alarmEntry aAlarm{
          .id = associatedTM, .cause = alarmState(), .sample = aTelemetry};
      alarmDestination.raiseAlarm(aAlarm);
    }
    // clear if we recovered
    else if ((aPreviousState == true) && aCurrentFlag == false) {
      alarmEntry aAlarm{
          .id = associatedTM, .cause = Alarm::empty, .sample = aTelemetry};
      alarmDestination.raiseAlarm(aAlarm);
    }
    // update last values so we can get the rate of change next time
    aSensor.lastValue = aTelemetry.readValue;
    aSensor.lastTMGenerationtime_ticks = aTelemetry.timestamp;
  }
}
