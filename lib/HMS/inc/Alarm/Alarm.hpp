/**
 * \file Alarm.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <cstdint>

/// CMAKE INCLUDES
// #include "HMS/version.h"

/// USER INCLUDES
#include "Common/HelperMacros.hpp"
#include "Telemetry/Telemetry.hpp"

// fixed after seeing issue in unit test, ENUM_AND_STRING isn't useful for
// bitshifted flags
enum class Alarm {
  empty = 0,
  too_low = 1,
  too_high = 1 << 1,
  rateofchange = 1 << 2,
  stale = 1 << 3
};

/// NAMESPACE

/// DEFINES

/// CODE
constexpr Alarm operator&(Alarm lhs, Alarm rhs) noexcept {
  return static_cast<Alarm>(static_cast<std::uint8_t>(lhs) &
                            static_cast<std::uint8_t>(rhs));
};

constexpr Alarm operator|(Alarm lhs, Alarm rhs) noexcept {
  return static_cast<Alarm>(static_cast<std::uint8_t>(lhs) |
                            static_cast<std::uint8_t>(rhs));
};

constexpr Alarm &operator|=(Alarm &lhs, Alarm rhs) noexcept {
  lhs = (lhs | rhs);
  return lhs;
};

/**
 * \brief alarmEntry to store/flag things
 * \note needs an ID (tmId), cause (Alarm), value (tm sample), timestamp
 * (tmsample)
 */
struct alarmEntry {
  TelemetryIds id{TelemetryIds ::MAX};
  Alarm cause{Alarm::empty};
  tmSample sample{};
};
