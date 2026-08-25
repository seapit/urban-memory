/**
 * \file AlarmQueue.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include "Alarm/Alarm.hpp"
#include <array>
#include <cstdint>

/// CMAKE INCLUDES
// #include "FDIR/version.h"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class AlarmQueue
 */
template <std::size_t Size> class AlarmQueue {
  static_assert((Size & (Size - 1)) == 0, "Use size equal to a power of 2");

public:
  /**
   * \brief try_push
   * attempts to push an item to the queue, users of this function are
   * responsible to ensure theeir data isn't dropped either must buffer, or try
   * again. spin locks to be avoided.
   *
   * \param rhsAlarm - input parameter
   * \return true  - on success
   * \return false  - on fail
   */
  bool try_push(const alarmEntry &rhsAlarm) noexcept {
    bool aReturn{false};

    // increment the number of fails to enter, we're using a bounded queue for
    // speed it still has its drawbacks
    if (numberOfEntries >= Size) {
      ++numberOfoverFlows;
    }
    // we are not full and can input
    else {
      auto index = (producerIndex & (Size - 1));
      buffer[index] = rhsAlarm;
      ++numberOfEntries;
      aReturn = true;
    }
    return aReturn;
  }

  /**
   * \brief try_pop
   * attempts to pull an item from the queue, users of this function must look @
   * return to see if a valid item is returned by reference
   *
   * \param rhsAlarm  - output parameter
   * \return true  - on success
   * \return false  - on fail
   */
  bool try_pop(const alarmEntry &rhsAlarm) noexcept {
    bool aReturn{false};
    if (numberOfEntries > 0) {
      auto index = (consumerIndex & (Size - 1));
      rhsAlarm = buffer[index];
      numberOfEntries--;
      aReturn = true;
    }
    return aReturn;
  }

  // protected:

private:
  std::array<alarmEntry, Size> buffer;

  std::size_t producerIndex{0};
  std::size_t consumerIndex{0};

  std::size_t numberOfEntries{0};
  // should track number of overflows
  std::size_t numberOfoverFlows{0};
};
