/**
 * \file AlarmReceiver.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
// #include <csignal>
// #include <cstdio>

/// CMAKE INCLUDES
// #include "HMS/version.h"

/// USER INCLUDES
#include "Alarm/Alarm.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class AlarmReceiver
 */
class AlarmReceiver {
public:
  virtual void raiseAlarm(const alarmEntry &rhs) noexcept = 0;
  /**
   * \brief Construct a new AlarmReceiver object
   *
   */
  virtual ~AlarmReceiver() = default;

  // protected:

  // private:
};
