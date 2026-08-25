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
#include "HMS/TelemetryDB.hpp"
#include "Telemetry/Telemetry.hpp"

ENUM_AND_STRING(alarm, (empty, 0), (too_low, 1), (too_high, 1 << 1),
                (rateofchange, 1 << 2), (stale, 1 < 3))

/// NAMESPACE

/// DEFINES

/// CODE
constexpr alarm operator&(alarm lhs, alarm rhs) noexcept {
  return static_cast<alarm>(static_cast<std::uint8_t>(lhs) &
                            static_cast<std::uint8_t>(rhs));
};

constexpr alarm operator|(alarm lhs, alarm rhs) noexcept {
  return static_cast<alarm>(static_cast<std::uint8_t>(lhs) |
                            static_cast<std::uint8_t>(rhs));
};

constexpr alarm &operator|=(alarm &lhs, alarm rhs) noexcept {
  lhs = (lhs | rhs);
  return lhs;
};

/**
 * \brief alarmEntry to store/flag things
 * \note needs an ID (tmId), cause (alarm), value (tm sample), timestamp
 * (tmsample)
 */
struct alarmEntry {
  TelemetryIds id{TelemetryIds ::MAX};
  alarm cause{alarm::empty};
  tmSample sample{};
};
