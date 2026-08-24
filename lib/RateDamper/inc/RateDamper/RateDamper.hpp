/**
 * \file RateDamper.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES

/// CMAKE INCLUDES
// #include "RateDamper/version.h"

/// USER INCLUDES
#include "RateDamper/Wheels/Configuration.hpp"

/// NAMESPACE

/// DEFINES

/// CODE

/**
 * \class RateDamper
 */
class RateDamper {
public:
  /**
   * \brief Construct a new RateDamper object
   *
   */
  RateDamper(const RateDamperConfiguration &rhsConfiguration)
      : damperConfig(rhsConfiguration) {};

  // This feels wrong, but anything more complicated wouldn't be realistic for
  // the interview question
  // Asume we provide a counter-torque to the current angular rate and the value
  // we want to achieve is 0 (for angular rate)
  double control(double rhsAngularRate_rad_sec) noexcept;

  double boundedTorqueValidation(double rhsInput) noexcept;

  // To use this for flags later in H&M question
  double boundedVoltageValidation(double rhsInput) noexcept;

  double boundedAngularRateValidation(double rhsInput) noexcept;
  // protected:

private:
  RateDamperConfiguration damperConfig;
};
