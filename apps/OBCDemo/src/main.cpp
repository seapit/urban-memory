/**
 * \file OBC.cpp
 * \brief Demo app. Runs the two things the brief asks for in ISOLATION, then
 *        runs them together through FDIR, which is my own extension.
 */

/// INCLUDES
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <format>
#include <string>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
#include "Alarm/Alarm.hpp"
#include "Alarm/AlarmReceiver.hpp"
#include "Databases/TelemetryDB.hpp"
#include "FDIR/FDIR.hpp"
#include "HMS/Configuration.hpp"
#include "HMS/HealthMonitor.hpp"
#include "Logger/Logger.hpp"
#include "RateDamper/RateDamper.hpp"
#include "RateDamper/Wheels/RW1000.hpp"
#include "Telemetry/Telemetry.hpp"

/// NAMESPACE

/// DEFINES
using milliseconds = std::chrono::milliseconds;
using TelemetryIds::RateController1_x_AngularRate, TelemetryIds::Temperature_1,
    TelemetryIds::RateController2_x_AngularRate, TelemetryIds::Temperature_2,
    TelemetryIds::RateController3_x_AngularRate, TelemetryIds::Voltage,
    TelemetryIds::MAX;

using milliseconds = std::chrono::milliseconds;

/// CODE

// ---- demo timing -----------------------------------------------------------
inline constexpr std::size_t systemTickRate_Hz{1000};
inline constexpr double tickPeriod_sec{1.0 / systemTickRate_Hz};
inline constexpr std::size_t simulationDuration_ticks{22000};

inline constexpr std::size_t tumbleInjection_tick{2000};       // t = 2 sec
inline constexpr std::size_t ratePublisherStops_tick{15000};   // t = 15 sec
inline constexpr std::size_t ratePublisherResumes_tick{18000}; // t = 18 sec

inline constexpr double angularRateConditionTime{2.5};

// monitorcfg
inline constexpr healthMonitorConfiguration demoConfig{
    .sensorConfigs = {{
        {.name = "AngularRate",
         .units = "rad/sec",
         .samplingPeriod_ticks = 10, // 100 Hz
         .lowLimit = -0.35,
         .highLimit = 0.35,
         .maxAbsoluteRateOfChange_UnitHz = 2.0,
         .telemetryToMonitor = {RateController1_x_AngularRate,
                                RateController2_x_AngularRate,
                                RateController3_x_AngularRate},
         .numberPermittedStaleUpdates = 3},
        {.name = "Temperature",
         .units = "degC",
         .samplingPeriod_ticks = 1000, // 1 Hz
         .lowLimit = -20.0,
         .highLimit = 65.0,
         .maxAbsoluteRateOfChange_UnitHz = 5.0,
         .telemetryToMonitor = {Temperature_1, Temperature_2, MAX},
         .numberPermittedStaleUpdates = 3},
        {.name = "Voltage",
         .units = "V",
         .samplingPeriod_ticks = 100, // 10 Hz
         .lowLimit = 25.0,
         .highLimit = 31.0,
         .maxAbsoluteRateOfChange_UnitHz = 0.5,
         .telemetryToMonitor = {Voltage, MAX, MAX},
         .numberPermittedStaleUpdates = 3},
    }}};

static_assert(demoConfig.isConfigurationRealistic(),
              "monitor configuration likely incorrect");

static std::string reflectCause(Alarm rhsCause) {
  std::string aReturn{};
  if (rhsCause.too_low) {
    aReturn += " too_low";
  }
  if (rhsCause.too_high) {
    aReturn += " too_high";
  }
  if (rhsCause.rateofchange) {
    aReturn += " rateofchange";
  }
  if (rhsCause.stale) {
    aReturn += " stale";
  }
  if (rhsCause.cleared) {
    aReturn += " cleared";
  }
  return aReturn.empty() ? std::string{" (none)"} : aReturn;
}

// intercept some alarms to be able to log them for the demo
class loggingAlarmReceiver final : public AlarmReceiver {
public:
  explicit loggingAlarmReceiver(const CommonTool::Logger &rhsLogger) noexcept
      : logger(rhsLogger) {};

  bool raiseAlarm(const alarmEntry &rhs) noexcept override {
    logger.info("    t=" + std::to_string(rhs.sample.timestamp) +
                " ticks  id=" + std::string{getTelemetryIdsstr(rhs.id)} +
                "  value=" + std::to_string(rhs.sample.readValue) +
                "  causes:" + reflectCause(rhs.cause));
    ++numberRaised;
    return true;
  }

  std::size_t numberRaised{0};

private:
  const CommonTool::Logger &logger;
};

// RateDamper in isolation
static void rateDamperDemo(const CommonTool::Logger &rhsLogger) {
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|------------------- RATE DAMPER -------------------|");
  rhsLogger.info("|---------------------------------------------------|");

  RateDamper aDamper{RW1000Configuration};

  const double aTorqueFloor = RW1000Configuration.outputTorque_Floor_N_m;

  rhsLogger.info("|See start of logs for wheel properties.            |");
  rhsLogger.info("|---------------------------------------------------|");

  // spans the linear region and the saturated one, both signs
  const double aRates[] = {0.0, 0.5, 1.0, 2.0, 2.5, 10.0, -0.5, -2.0, -10.0};
  // std::to_string always gives six decimals and a variable width integer
  // part, so nothing lines up. std::format takes the width AND the precision,
  // which is what keeps the columns square as values change sign or magnitude.
  rhsLogger.info("|rate                                  output torque|"));
  rhsLogger.info("|---------------------------------------------------|");

  for (const double aRate : aRates) {
    const double aTorque = aDamper.control(aRate);
    const bool aSaturated =
        ((aTorque >= aTorqueFloor) || (aTorque <= (0 - aTorqueFloor)));

    rhsLogger.info(std::format("|{:>13.4f}  {:<11}{:>25.4f}|", aRate,
                               (aSaturated ? "[clamped]" : ""), aTorque));
  }
}

static void hmsDemo(const CommonTool::Logger &rhsLogger) {
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|---------------- HEALTH MONITORING ----------------|");
  rhsLogger.info("|---------------------------------------------------|");

  // init everything based off unit tests
  CriticalTelemetryDB aTelemetry{};
  loggingAlarmReceiver aCollector{rhsLogger};
  HealthMonitor aMonitor{demoConfig, aTelemetry, aCollector};

  rhsLogger.info("|See start of logs for HMS properties.              |");
  rhsLogger.info("|---------------------------------------------------|");

  // indicate at which times we'll introduce errors for each of the channels
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|-------------- FAULT INJECTION TIMES --------------|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Name     Time Interval(sec)     Time Interval(tick)|");
  rhsLogger.info("|Angular Rate      [1-2]           [1000-2000 ticks]|");
  rhsLogger.info("|Temperature       [3-5]           [3000-5000 ticks]|");
  rhsLogger.info("|Voltage           [6-7]           [6000-7000 ticks]|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|-------------- STALE INSERTION TIMES --------------|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Name     Time Interval(sec)     Time Interval(tick)|");
  rhsLogger.info("|Angular Rate     [8-8.5]          [8000-8500 ticks]|");
  rhsLogger.info("|---------------------------------------------------|");
  for (std::size_t aTick = 10; aTick <= 9000; aTick += 10) {

    // ---- channel 0, the angular rates ----
    const bool aRateFaulted = ((aTick >= 1000) && (aTick < 2000));
    const bool aRatePublishing = ((aTick < 8000) || (aTick >= 8500));

    if (aRatePublishing) {
      aTelemetry.publish(TelemetryIds::RateController1_x_AngularRate,
                         tmSample{(aRateFaulted ? 0.40 : 0.10), aTick});
    }
    // publish data for other 'rate controllers'
    aTelemetry.publish(TelemetryIds::RateController2_x_AngularRate,
                       tmSample{0.05, aTick});
    aTelemetry.publish(TelemetryIds::RateController3_x_AngularRate,
                       tmSample{0.04, aTick});

    // get condition to output temperature & publish
    const bool aTempFaulted = ((aTick >= 3000) && (aTick < 5000));
    aTelemetry.publish(TelemetryIds::Temperature_1,
                       tmSample{(aTempFaulted ? 80.0 : 21.0), aTick});
    aTelemetry.publish(TelemetryIds::Temperature_2, tmSample{22.5, aTick});

    // get condition to output voltage & publish
    const bool aVoltFaulted = ((aTick >= 6000) && (aTick < 7000));
    aTelemetry.publish(TelemetryIds::Voltage,
                       tmSample{(aVoltFaulted ? 22.0 : 28.0), aTick});

    // simulate a HMS task being run where it must check all the different
    // sensors
    aMonitor.checkMonitorCondition(milliseconds{static_cast<long>(aTick)});
  }
  rhsLogger.info("|Alarms raised:                                    " +
                 std::to_string(aCollector.numberRaised) + "|");
  rhsLogger.info("|---------------------------------------------------|");
}

static void fdirDemo(const CommonTool::Logger &rhsLogger) {

  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|----------------------- FDIR ----------------------|");
  rhsLogger.info("|---------------------------------------------------|");

  CriticalTelemetryDB aTelemetry{};
  RateDamper aDamper{RW1000Configuration};

  const fdirConfiguration aFdirConfiguration{
      .actionableTelemetries = {{
          {RateController1_x_AngularRate, &aDamper},
      }}};

  FDIR aFdir{aFdirConfiguration, aTelemetry};
  HealthMonitor aMonitor{demoConfig, aTelemetry, aFdir};
}

void printsomeInfo(const CommonTool::Logger &rhsLogger) {
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|------------------- RATE DAMPER -------------------|");
  rhsLogger.info("|-------------------  Wheel Data -------------------|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Maximum Angular Momentum Nms                 | 11.0|");
  rhsLogger.info("|Output Torque Floor                          |  1.0|");
  rhsLogger.info("|Angular Rate (max) rev/min                   |200.0|");
  rhsLogger.info("|Voltage V                                    | 28.0|");
  rhsLogger.info("|Voltage Variation V                          |  3.0|");
  rhsLogger.info("|steady State Power Consumption               |160.0|");
  rhsLogger.info("|dampingGain N*m*sec/rad  (my selection)      |  0.5|");
  rhsLogger.info("|mass kg                                      | 10.0|");
  rhsLogger.info("|diameter mm                                  |337.0|");
  rhsLogger.info("|height mm                                    |121.0|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|---------------- HEALTH MONITORING ----------------|");
  rhsLogger.info("|----------------   Demo Sensors    ----------------|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Sensor 1                                           |");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Name:                                  Angular Rate|");
  rhsLogger.info("|Units:                                      rad/sec|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Sampling Period (icks)                       |   10|");
  rhsLogger.info("|Low Limit                                    |-0.35|");
  rhsLogger.info("|High Limit                                   | 0.35|");
  rhsLogger.info("|Rate Limit                                   |  2.0|");
  rhsLogger.info("|numberPermittedStaleUpdates                  |    3|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Associated TM:                                     |");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("| Name:                                           ID|");
  rhsLogger.info("| RateController1_x_AngularRate                    0|");
  rhsLogger.info("| RateController2_x_AngularRate                    2|");
  rhsLogger.info("| RateController3_x_AngularRate                    4|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Sensor 2                                           |");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Name:                                   Temperature|");
  rhsLogger.info("|Units:                                         degC|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Sampling Period (icks)                       | 1000|");
  rhsLogger.info("|Low Limit                                    |-20.0|");
  rhsLogger.info("|High Limit                                   | 65.0|");
  rhsLogger.info("|Rate Limit                                   |  5.0|");
  rhsLogger.info("|numberPermittedStaleUpdates                  |    3|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Associated TM:                                     |");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("| Name:                                           ID|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("| Temperature_1                                |   1|");
  rhsLogger.info("| Temperature_2                                |   3|");
  rhsLogger.info("| N/A                            (MAX OF ENUM) |   6|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Sensor 3                                           |");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Name:                                       Voltage|");
  rhsLogger.info("|Units:                                        volts|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Sampling Period (icks)                       |  100|");
  rhsLogger.info("|Low Limit                                    | 11.5|");
  rhsLogger.info("|High Limit                                   | 12.5|");
  rhsLogger.info("|Rate Limit                                   |  0.5|");
  rhsLogger.info("|numberPermittedStaleUpdates                  |    3|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Associated TM:                                     |");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("| Name:                                           ID|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("| Voltage                                    |     1|");
  rhsLogger.info("| N/A                          (MAX OF ENUM) |     6|");
  rhsLogger.info("| N/A                          (MAX OF ENUM) |     6|");
  rhsLogger.info("|---------------------------------------------------|");
};

int main() {
  CommonTool::Logger aLogger{"OBCDemo"};
  printsomeInfo(aLogger);
  rateDamperDemo(aLogger);
  hmsDemo(aLogger);
  fdirDemo(aLogger);

  return 0;
}
