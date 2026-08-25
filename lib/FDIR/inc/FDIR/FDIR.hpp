/**
 * \file FDIR.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
// #include <csignal>
// #include <cstdio>

/// CMAKE INCLUDES
// #include "FDIR/version.h"

/// USER INCLUDES
#include "Alarm/AlarmReceiver.hpp"
#include "HMS/Alarm.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class FDIR
 */
class FDIR : public AlarmReceiver {
public:
  /**
   * \brief Construct a new FDIR object
   *
   */
  FDIR();

  void alarm(Alarm rhsInput) noexcept override {}

  // protected:

  // private:
};
