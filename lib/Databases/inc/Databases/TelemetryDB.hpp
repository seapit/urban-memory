/**
 * \file TelemetryDB.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <array>

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
    // guard clause to ensure safety of what we input
    if (rhsId != TelemetryIds::MAX) {
      criticalTMEntries[sizecast(rhsId)].update(rhsSample);
    }
  }

  inline const tmSample getLatest(TelemetryIds rhsId) const {
    // default initialize a sample for our return
    tmSample returnSample{};

    // if it is valid, update it with whatever's in the critical storage
    if (rhsId < TelemetryIds::MAX) {
      returnSample = criticalTMEntries[sizecast(rhsId)].getLatest();
    }

    // return either the empty sample or a valid one
    // do not expect to return a TelemtryId as we should really try to optimize
    // our code to... just not, enum class used to ensure we only can give
    // defined values and not pass numerical data types to represent an index
    return returnSample;
  }

  // protected:

private:
  std::array<PingPongBuffer<tmSample>, sizecast(TelemetryIds::MAX)>
      criticalTMEntries;
};
