/**
 * \file RateDamper.cpp
 * \brief A brief description of what this file is.
 */

/// INCLUDES
#include <algorithm>
#include <cmath>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
#include "Common/HelperMacros.hpp"
#include "RateDamper/RateDamper.hpp"

/// NAMESPACE

/// DEFINES

/// CODE
double RateDamper::control(double rhsAngularRate_rad_sec) noexcept {
  // Can a reaction wheel exceed it's maximum angular rate?
  // Does it catastrophically fail?
  [[maybe_unused]] double aBoundedInputRate =
      boundedAngularRateValidation(rhsAngularRate_rad_sec);

  aRetVal = damperConfig.dampingGain_N_m_s_per_rad * rhsAngularRate_rad_sec;

  return boundedTorqueValidation(aReturn);
};

double RateDamper::boundedTorqueValidation(double rhsInput) noexcept {
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
  // the maximum torque we get at full power would be 1Nm
  // our damping factor is .5. I think that what we can actually bound for is
  // the body rate
  double aMaximumAngularInputRate = damperConfig.outputTorque_Floor_N_m /
                                    damperConfig.dampingGain_N_m_s_per_rad;

  aRetVal = std::clamp(rhsInput, (0 - aMaximumAngularInputRate),
                       aMaximumAngularInputRate);

  // from CPPref, we must avoid NaNs so just check that it's infinite and then
  // update accordingly
  if (std::isfinite(rhsInput) == false) {
    aReturn = 0.0;
  }

  return aReturn;
}
// protected:
