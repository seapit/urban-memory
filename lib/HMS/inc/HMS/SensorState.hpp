/**
 * \file State.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <cstddef>

/// CMAKE INCLUDES
// #include "HMS/version.h"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class State
 */
struct sensorState {
  // Explicitly from question 2b
  // ensure these values are updated if we ever have an issue
  bool outOfRange_Low{false};
  bool outOfRange_High{false};
  bool outOfRange_RateOfChange{false};

  bool staleValues{false};
  std::size_t numberOfStale{0};

  double lastValue{0.0};
  double lastRateOfChange{0.0};

  std::size_t lastTMGenerationtime_ticks{0};
  std::size_t numberOfSamples{0};
  bool hasfirstSample{false};
};
