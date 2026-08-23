/**
 * \file Configuration.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <cstdint>

#include <numbers>
/// CMAKE INCLUDES
// #include "RateDamper/version.h"
#include "Common/Constants.hpp"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES

/// CODE

struct RateDamperConfiguration {
  // Limits
  double maximumAngularMomentum_N_m_s{0};
  // defer to floor - data sheet is odd and indicates >= 1 N*m,
  // must be missing something since this shouldn't be a max
  double outputTorque_Floor_N_m{0};
  double maximumAngularRate_rev_min{0};

  double nominalVoltage_V{0.0};
  double nominalVoltageVariation_V{0};
  double steadyStatePowerConsumption_W{0.0};
  double maximumPowerConsumption_W{0.0};

  // Control
  // damping factor has torque = k*w
  // (math is probably off here)
  // any value >0 would converge? I think?
  // lets go with a damping factor between [0.0, 1.0]
  // https://ntrs.nasa.gov/api/citations/19980236616/downloads/19980236616.pdf
  // page 13 #3 indicates the use .2 or .5, initial assumption is correct

  // torque = Nm, w = rad/sec
  // must have units of N * m * sec/rad
  //                              N m / (rad/sec) = N m s / rad
  double dampingGain_N_m_s_per_rad{0.0};

  // Physical properties, not needed but lets do a complete job
  double mass_kg{0.0};
  double diameter_mm{0.0};
  double height_mm{0.0};

  // helper function
  constexpr double convertAngularRate_rpm_to_rad_sec() const noexcept {
    return ((maximumAngularRate_rev_min * seconds_to_minute) * (2 * pi));
  };

  // Lets add a sanity check
  // hopefully brownie points
  //  W = 1 N*m/s ,
  // P_max = torque * AngularVel_max
  constexpr bool isRealisticWheel() const {
    return (maximumPowerConsumption_W >=
            (outputTorque_Floor_N_m * convertAngularRate_rpm_to_rad_sec()));
  };
};
