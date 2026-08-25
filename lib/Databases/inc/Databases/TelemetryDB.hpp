/**
 * \file TelemetryDB.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
/// CMAKE INCLUDES
// #include "Lib1/version.h"

/// USER INCLUDES
#include "Common/HelperMacros.hpp"
#include "PingPongBuffer/PingPongBuffer.hpp"

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

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class CriticalTelemetryDB
 */
class CriticalTelemetryDB {
public:
  /**
   * \brief Construct a new TelemetryDB object
   *
   */
  inline void publish(TelemetryIds rhsId, const tmSample &rhsSample) {
    criticalTMEntries[static_cast<std::size_t>(rhsId)].update(rhsSample);
  }

  inline tmSample getLatest(TelemetryIds rhsId) {
    return criticalTMEntries[static_cast<std::size_t>(rhsId)].getLatest();
  }

  // protected:

private:
  std::array<PingPongBuffer<tmSample>,
             static_cast<std::size_t>(TelemetryIds::MAX)>
      criticalTMEntries;
};
