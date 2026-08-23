/**
 * \file RateDamper.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
// #include <csignal>
// #include <cstdio>
#include <algorithm>
#include <cstdint>

/// CMAKE INCLUDES
// #include "RateDamper/version.h"

/// USER INCLUDES
#include "Common/HelperMacros.hpp"
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
  double control(double rhsAngularRate_rad_sec) noexcept {
    // Can a reaction wheel exceed it's maximum angular rate?
    // Does it catastrophically fail?
    double aBoundedInputRate =
        boundedAngularRateValidation(rhsAngularRate_rad_sec);

    aRetVal = damperConfig.dampingGain_N_m_s_per_rad * aBoundedInputRate;

    return boundedTorqueValidation(aReturn);
  };

  double boundedTorqueValidation(double rhsInput) noexcept {
    // std::clamp returns a ref

    aRetVal = std::clamp(rhsInput, (0 - damperConfig.outputTorque_Floor_N_m),
                         damperConfig.outputTorque_Floor_N_m);
    return aReturn;
  };

  // To use this for flags later in H&M question
  double boundedVoltageValidation(double rhsInput) noexcept {
    aRetVal = std::clamp(rhsInput,
                         (damperConfig.nominalVoltage_V -
                          damperConfig.nominalVoltageVariation_V),
                         (damperConfig.nominalVoltage_V +
                          damperConfig.nominalVoltageVariation_V));
    return aReturn;
  };

  double boundedAngularRateValidation(double rhsInput) noexcept {
    double aMaximumRate = damperConfig.convertAngularRate_rpm_to_rad_sec();
    aRetVal = std::clamp(rhsInput, (0 - aMaximumRate), aMaximumRate);
    return aReturn;
  }
  // protected:

private:
  RateDamperConfiguration damperConfig;
};
