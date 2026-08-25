/**
 * \file Configuration.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <array>
#include <cstdint>
#include <utility>
/// CMAKE INCLUDES
// #include "FDIR/version.h"

/// USER INCLUDES
#include "Alarm/Alarm.hpp"
#include "RateDamper/RateDamper.hpp"
#include "Telemetry/Telemetry.hpp"

/// NAMESPACE

/// DEFINES

/// CODE

struct fdirConfiguration {
  std::array<std::pair<TelemetryIds, RateDamper>, 1> eligibleTelemetries;
};