/**
 * \file Template.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
// #include <csignal>
// #include <cstdio>
#include <string>
/// CMAKE INCLUDES
// #include "Lib1/version.h"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES

/// CODE

struct sensorDefinition {
  std::string name{};
  std::string units{};

  double lowLimit{0.0};
  double highLimit{0.0};

  // rate of change in units/sec
  double maxAbsoluteRateOfChange_UnitHz{0.0};

  // placeholder
  std::size_t numberOfRollingAverageSamples{0};
};
