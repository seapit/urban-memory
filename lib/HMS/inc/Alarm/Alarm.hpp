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
#pragma pack(push, 1)
struct Alarm {
  std::uint8_t too_low : 1 {0};
  std::uint8_t too_high : 1 {0};
  std::uint8_t rateofchange : 1 {0};
  std::uint8_t stale : 1 {0};
  std::uint8_t cleared : 1 {0};
};
#pragma pack(pop)

/// NAMESPACE

/// DEFINES

/// CODE
// constexpr Alarm operator&(Alarm lhs, Alarm rhs) noexcept {
//   return static_cast<Alarm>(static_cast<std::uint8_t>(lhs) &
//                             static_cast<std::uint8_t>(rhs));
// };

// constexpr Alarm operator|(Alarm lhs, Alarm rhs) noexcept {
//   return static_cast<Alarm>(static_cast<std::uint8_t>(lhs) |
//                             static_cast<std::uint8_t>(rhs));
// };

// constexpr Alarm &operator|=(Alarm &lhs, Alarm rhs) noexcept {
//   lhs = (lhs | rhs);
//   return lhs;
// };

/**
 * \brief alarmEntry to store/flag things
 * \note needs an ID (tmId), cause (Alarm), value (tm sample), timestamp
 * (tmsample)
 */
struct alarmEntry {
  TelemetryIds id{TelemetryIds ::MAX};
  Alarm cause{
      .too_low = 0, .too_high = 0, .rateofchange = 0, .stale = 0, .cleared = 0};
  tmSample sample{};
};
