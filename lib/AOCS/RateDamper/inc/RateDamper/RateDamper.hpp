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
 * \note  this is the technical task question #2a
 * must: take an angular velocity as input and output a torque
 * \note will use a double to output
 */
class RateDamper {
public:
  /**
   * \brief Construct a new RateDamper object
   *
   */
  RateDamper(const RateDamperConfiguration &rhsConfiguration)
      : damperConfig(rhsConfiguration) {};

  /**
   * \brief control
   *
   * \param rhsAngularRate_rad_sec
   * \note This feels wrong, but anything more complicated wouldn't be realistic
   * for the interview question Asume we provide a counter-torque to the current
   * angular rate and the value we want to achieve is 0 (for angular rate) I
   * am taking output in the pure sense of the word, a torque value is returned,
   * should I make a stub to command over an interface instead?
   * \return double
   */
  double control(double rhsAngularRate_rad_sec) noexcept;

  /**
   * \brief boundedTorqueValudation
   * Exists to ensure the torque doesn't exceed the maximum of the wheel

   * \param rhsInput - N*m units, input is torque
   * \return double
   */
  double boundedTorqueValidation(double rhsInput) noexcept;

  /**
   * \brief boundedVoltageValidation
   * o use this for flags later in H&M question
   *
   * \param rhsInput - V units, input is voltage
   * \return double
   */
  double boundedVoltageValidation(double rhsInput) noexcept;

  /**
   * \brief boundedAngularRateValidation
   * Exists to ensure the angular velocity doesn't exceed the maximum of the
   wheel
   *
   * \param rhsInput - rad*sec units, input is angular velocity
   * \return double
   */
  double boundedAngularRateValidation(double rhsInput) noexcept;
  // protected:

private:
  RateDamperConfiguration damperConfig;
};
