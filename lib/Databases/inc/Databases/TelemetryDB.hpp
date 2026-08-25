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
#include "Telemetry/Telemetry.hpp"

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
