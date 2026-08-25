/**
 * \file FDIR.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <cstdint>

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
  static inline constexpr std::size_t alarmBufferSize{4};

public:
  /**
   * \brief Construct a new FDIR object
   *
   */
  FDIR();

  void alarm(Alarm rhsInput) noexcept override;

  void executionLoop() noexcept;

  // protected:

  // private:
};
