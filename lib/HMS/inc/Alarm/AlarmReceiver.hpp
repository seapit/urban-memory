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
#include "HMS/Alarm.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class AlarmReceiver
 */
class AlarmReceiver {
public:
  virtual void raiseAlarm(const alarm &rhs) noexcept = 0;
  /**
   * \brief Construct a new AlarmReceiver object
   *
   */
  virtual ~AlarmReceiver();

  // protected:

  // private:
};
