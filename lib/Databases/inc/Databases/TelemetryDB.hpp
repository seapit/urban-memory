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
#include "Databases/PingPongBuffer.hpp"
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
    criticalTMEntries[sizecast(rhsId)].update(rhsSample);
  }

  inline const tmSample getLatest(TelemetryIds rhsId) const {
    return criticalTMEntries[sizecast(rhsId)].getLatest();
  }

  // protected:

private:
  std::array<PingPongBuffer<tmSample>, sizecast(TelemetryIds::MAX)>
      criticalTMEntries;
};
