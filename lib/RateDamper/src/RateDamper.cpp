/**
 * \file RateDamper.cpp
 * \brief A brief description of what this file is.
 */

/// INCLUDES
#include <algorithm>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
#include "RateDamper/RateDamper.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
double RateDamper::control(double rhsAngularRate_rad_sec) noexcept {
  // Can a reaction wheel exceed it's maximum angular rate?
  // Does it catastrophically fail?
  double aBoundedInputRate =
      boundedAngularRateValidation(rhsAngularRate_rad_sec);

  aRetVal = damperConfig.dampingGain_N_m_s_per_rad * aBoundedInputRate;

  return boundedTorqueValidation(aReturn);
};

double RateDamper::boundedTorqueValidation(double rhsInput) noexcept {
  // std::clamp returns a ref

  aRetVal = std::clamp(rhsInput, (0 - damperConfig.outputTorque_Floor_N_m),
                       damperConfig.outputTorque_Floor_N_m);
  return aReturn;
};

// To use this for flags later in H&M question
double RateDamper::boundedVoltageValidation(double rhsInput) noexcept {
  aRetVal = std::clamp(
      rhsInput,
      (damperConfig.nominalVoltage_V - damperConfig.nominalVoltageVariation_V),
      (damperConfig.nominalVoltage_V + damperConfig.nominalVoltageVariation_V));
  return aReturn;
};

double RateDamper::boundedAngularRateValidation(double rhsInput) noexcept {
  double aMaximumRate = damperConfig.convertAngularRate_rpm_to_rad_sec();
  aRetVal = std::clamp(rhsInput, (0 - aMaximumRate), aMaximumRate);
  return aReturn;
}
// protected:
