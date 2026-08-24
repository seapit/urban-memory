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
#include "PingPongBuffer/PingPongBuffer.hpp"

// this is a helper function I didn't create (from Stack Overflow)
// it works well for my home projects
ENUM_AND_STRING(TelemetryIds, (RateController), (Temperature), (Voltage))

struct tmSample {
  double readValue{0.0};
  std::size_t timestamp{0};
};

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
  void publish(TelemetryIds rhsId, tmSample rhsSample) {}

  // protected:

  // private:
  std::array < PingPongBuffer<tmSample>,
      static_cast<std::size_t(TelemetryIds::MAX)> criticalTMEntries;
};
