/**
 * \file TelemetryDB.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
// #include <csignal>
// #include <cstdio>

/// CMAKE INCLUDES
// #include "Lib1/version.h"

/// USER INCLUDES
#include "Common/HelperMacros.hpp"

// this is a helper function I didn't create (from Stack Overflow)
// it works well for my home projects
ENUM_EXACT_BASE_AND_STRING(TelemetryIds, RateController, Temperature, Voltage)

struct TelemetrySet {};

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class TelemetryDB
 */
class TelemetryDB {
public:
  /**
   * \brief Construct a new Template object
   *
   */
  TelemetryDB();

  // protected:

  // private:
};
