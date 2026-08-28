/**
 * \file OBC.cpp
 * \brief Demo app. Runs the two things the brief asks for in ISOLATION, then
 *        runs them together through FDIR, which is my own extension.
 */

/// INCLUDES
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <format>
#include <numbers>
#include <string>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
#include "Alarm/Alarm.hpp"
#include "Alarm/AlarmReceiver.hpp"
#include "Common/Constants.hpp"
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
inline constexpr std::size_t simulationDuration_short_ticks{2000};

// A rate that stays INSIDE the position limits but moves too fast, so the
// only cause it can raise is rateofchange. 20 samples per period at the rate
// channel's 10 tick period puts omega0 at 10*pi rad/sec.
inline constexpr double rateOscillationAmplitude_rad_sec{0.30};
inline constexpr std::size_t simulationDuration_med_ticks{10000};
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
  // every branch above prepends a space, so drop the leading one - a column
  // that starts with a stray space is not aligned, it just looks aligned
  return aReturn.empty() ? std::string{"(none)"} : aReturn.substr(1);
}

// intercept some alarms to be able to log them for the demo
class loggingAlarmReceiver final : public AlarmReceiver {
public:
  explicit loggingAlarmReceiver(const CommonTool::Logger &rhsLogger) noexcept
      : logger(rhsLogger) {};

  // the column widths match the header printed by hmsDemo, so the alarms
  // land under their titles as they arrive
  static void printHeader(const CommonTool::Logger &rhsLogger) {
    rhsLogger.info(std::format("|{:>6}       {:>3}  {:>8}  {:<23}|", "tick",
                               "TM", "value", "causes"));
    rhsLogger.info("|---------------------------------------------------|");
  }

  bool raiseAlarm(const alarmEntry &rhs) noexcept override {
    // the numeric TM id, not the name - the names are 29 characters and would
    // push the causes column outside the frame
    logger.info(std::format("|{:>6}       {:>3}  {:>8.2f}  {:<23}|",
                            rhs.sample.timestamp, sizecast(rhs.id),
                            rhs.sample.readValue, reflectCause(rhs.cause)));
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

  rhsLogger.info(std::format("|{:>6}       {:>10}  {:>10}  {:<14}|", "t(s)",
                             "rate", "torque", ""));
  rhsLogger.info("|---------------------------------------------------|");

  constexpr double aStep_sec{0.01};
  double aRate_rad_sec{2.5};

  for (std::size_t aStep = 0; aStep <= simulationDuration_short_ticks;
       ++aStep) {
    const double aTorque = aDamper.control(aRate_rad_sec);

    // every step is 10 ms, 100 steps = 10*100 = 1000ms = 1 sec
    // output on each 1 sec interval
    if ((aStep % 100) == 0) {
      const bool aSaturated =
          ((aTorque >= aTorqueFloor) || (aTorque <= (0 - aTorqueFloor)));
      rhsLogger.info(
          std::format("|{:>6.1f}       {:>10.4f}  {:>10.4f}  {:<14}|",
                      (10 * tickPeriod_sec * aStep), aRate_rad_sec, aTorque,
                      (aSaturated ? "[clamped]" : "")));
    }

    // unit inertia, so omega_dot is numerically the torque
    aRate_rad_sec += (aTorque * aStep_sec);
  }

  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("");
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

  // indicate at which times we'll introduce errors for each of the channels
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|-------------- FAULT INJECTION TIMES --------------|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Name     Time Interval(sec)     Time Interval(tick)|");
  rhsLogger.info("|Angular Rate      [1-2]           [1000-2000 ticks]|");
  rhsLogger.info("|Temperature       [3-5]           [3000-5000 ticks]|");
  rhsLogger.info("|Voltage           [6-7]           [6000-7000 ticks]|");
  rhsLogger.info("|Angular Rate      [9-10]         [9000-10000 ticks]|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|-------------- STALE INSERTION TIMES --------------|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|Name     Time Interval(sec)     Time Interval(tick)|");
  rhsLogger.info("|Angular Rate     [8-8.5]          [8000-8500 ticks]|");
  rhsLogger.info("|---------------------------------------------------|");
  loggingAlarmReceiver::printHeader(rhsLogger);

  for (std::size_t aTick = 10; aTick <= simulationDuration_med_ticks;
       aTick += 10) {

    // set ticks where we inject faults
    const bool aRateFaulted = ((aTick >= 1000) && (aTick < 2000));
    const bool aRatePublishCondition = ((aTick < 8000) || (aTick >= 8500));
    const bool aOscillationTickRange = ((aTick >= 9000) && (aTick < 10000));

    if (aRatePublishCondition) {
      const double aSteadyRate = (aRateFaulted ? 0.40 : 0.10);
      const double aOscillationRate =
          (rateOscillationAmplitude_rad_sec *
           std::sin(10.0 * std::numbers::pi *
                    (static_cast<double>(aTick) / 1000.0)));

      aTelemetry.publish(
          TelemetryIds::RateController1_x_AngularRate,
          tmSample{(aOscillationTickRange ? aOscillationRate : aSteadyRate),
                   aTick});
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
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info(std::format("|{:<38}     {:>8}|",
                             "Alarms raised:", aCollector.numberRaised));
  rhsLogger.info("|---------------------------------------------------|");

  rhsLogger.info("");
}

/**
 * \class alarmInterceptor
 * \brief Sits between the monitor and FDIR so the demo can report WHICH alarm
 *        drove a command, then forwards it untouched.
 * \note FDIR consumes alarms internally, so without this the log could show
 *       the torque but not the reason for it.
 */
class alarmInterceptor final : public AlarmReceiver {
public:
  explicit alarmInterceptor(AlarmReceiver &FDIR) noexcept : next(FDIR) {};

  bool raiseAlarm(const alarmEntry &rhs) noexcept override {
    lastCause = rhs.cause;
    raisedThisTick = true;
    return next.raiseAlarm(rhs);
  }

  Alarm lastCause{};
  bool raisedThisTick{false};

private:
  AlarmReceiver &next;
};

static void fdirDemo(const CommonTool::Logger &rhsLogger) {

  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("|----------------------- FDIR ----------------------|");
  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("| Commands rate damper from alarms                  |");
  rhsLogger.info("| See start of logs for wheel properties and        |");
  rhsLogger.info("| telemetry.                                        |");
  rhsLogger.info("|---------------------------------------------------|");

  CriticalTelemetryDB aTelemetry{};
  RateDamper aDamper{RW1000Configuration};

  const fdirConfiguration aFdirConfiguration{
      .actionableTelemetries = {{
          {RateController1_x_AngularRate, &aDamper},
      }}};

  FDIR aFdir{aFdirConfiguration, aTelemetry};
  alarmInterceptor aTap{aFdir};
  HealthMonitor aMonitor{demoConfig, aTelemetry, aTap};

  rhsLogger.info(std::format("|{:>8}  {:>18}  {:<21}|", "t(s)",
                             "commanded torque", "alarm"));
  rhsLogger.info("|---------------------------------------------------|");

  for (std::chrono::milliseconds time = std::chrono::milliseconds{10};
       sizecast(time.count()) <= simulationDuration_ticks;
       time += std::chrono::milliseconds{10}) {

    // reset output value
    aTap.raisedThisTick = false;

    // get the number of seconds
    const double aSeconds = (static_cast<double>(time.count()) / 1000.0);

    // get tick in size_t
    const std::size_t aTick = sizecast(time.count());

    // condition to insert a diff in the angular rate
    const bool aTumbleCondition_tick = (aTick >= tumbleInjection_tick);

    // conditions to force a stale mode
    const bool aPublishConditionTickRange =
        ((aTick < ratePublisherStops_tick) ||
         (aTick >= ratePublisherResumes_tick));

    // condition to allow a
    const bool aOscillationTickRange = (aTick < (tumbleInjection_tick / 2));

    const double aOscillationRate =
        (rateOscillationAmplitude_rad_sec *
         std::sin(10.0 * std::numbers::pi * aSeconds));

    const double aSteadyRate =
        (aOscillationTickRange ? aOscillationRate : 0.10);

    if (aPublishConditionTickRange) {
      aTelemetry.publish(
          RateController1_x_AngularRate,
          tmSample{
              (aTumbleCondition_tick ? angularRateConditionTime : aSteadyRate),
              aTick});
    }

    aMonitor.checkMonitorCondition(time);

    const double aTorque = aFdir.tick(time);

    // a row per ALARM, since an alarm is the only thing that changes what
    // FDIR does - printing every tick would bury them
    if (aTap.raisedThisTick) {
      rhsLogger.info(std::format("|{:>8.3f}  {:>18.4f}  {:<21}|", aSeconds,
                                 aTorque, reflectCause(aTap.lastCause)));
    }
  }

  rhsLogger.info("|---------------------------------------------------|");
  rhsLogger.info("");
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
  rhsLogger.info("");
};

int main() {
  CommonTool::Logger aLogger{"OBCDemo"};
  printsomeInfo(aLogger);
  rateDamperDemo(aLogger);
  hmsDemo(aLogger);
  fdirDemo(aLogger);

  return 0;
}
