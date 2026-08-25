/**
 * \file Telemetry.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES

/// CMAKE INCLUDES
// #include "Telemetry/version.h"

/// USER INCLUDES
#include "Common/HelperMacros.hpp"

/// NAMESPACE

/// DEFINES

/// CODE

// this is a helper function I didn't create (from Stack Overflow)
// it works well for my home projects
// this exists to create an enum class automatically and a string_View I can use
// for reflection
// there are libraries that do this better.
ENUM_AND_STRING(TelemetryIds, (RateController), (Temperature), (Voltage))

// What's stored in the database
struct tmSample {
  double readValue{0.0};
  std::size_t timestamp{0};
};
