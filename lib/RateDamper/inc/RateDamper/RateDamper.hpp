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
#include <numbers>

/// CMAKE INCLUDES
// #include "RateDamper/version.h"

/// USER INCLUDES
#include "Common/HelperMacros.hpp"

/// NAMESPACE

/// DEFINES

/// CODE

// this is ugly but i don't know if there's a ready made library for conversions
static inline constexpr std::size_t seconds_per_minute{
    60}; // units of sec/minute
static inline constexpr double seconds_to_minute{
    1.0 / seconds_per_minute}; // units of minute/sec

static inline constexpr double mm_per_m{1000};          // units of mm/m
static inline constexpr double mm_to_m{1.0 / mm_per_m}; // units of m/mm

static inline constexpr double pi{std::numbers::pi};

// using this reaction wheel
// https://satsearch.co/products/oce-technology-rw1000-reaction-wheel
// maximum angular momentum 11 N m s
// maximum output torque // ≥ 1 N m
// maximum speed ±1200 rpm
// voltage 28 ±3V
// power consumption (steady state) ≤ 15 W
// power consumption (max) 160W ≤ 160 W
// data interface: CAN 2.0B RS422
// diameter 337 mm
// height 121 mm
// mass  ≤10 kg
struct RateDamperConfiguration {
  // Limits
  static inline constexpr double maximumAngularMomentum_N_m_s{11};
  // defer to floor - data sheet is odd and indicates >= 1 N*m,
  // must be missing something since this shouldn't be a max
  static inline constexpr double outputTorque_Floor_N_m{1.0};
  static inline constexpr double maximumAngularRate_rev_min{1200};
  static inline constexpr double maximumAngularRate_rad_sec{
      // Rev/min * min/sec *rad/rev = Rev/sec * rad/rev = rad/sec
      (maximumAngularRate_rev_min * seconds_to_minute) * (2 * pi)};

  static inline constexpr double nominalVoltage_V{28.0};
  static inline constexpr double nominalVoltageVariation_V{3.0};
  static inline constexpr double steadyStatePowerConsumption_W{15.0};
  static inline constexpr double maximumPowerConsumption_W{160.0};

  // Control
  // damping factor has torque = k*w
  // (math is probably off here)
  // any value >0 would converge? I think?
  // lets go with a damping factor between [0.0, 1.0]

  // torque = Nm, w = rad/sec
  // must have units of N * m * sec/rad
  //                              N m / (rad/sec) = N m s / rad
  static inline constexpr double dampingGain_N_m_s_per_rad{0.5};

  // Physical properties, not needed but lets do a complete job
  static inline constexpr double mass_kg{10};
  static inline constexpr double diameter_mm{337};
  static inline constexpr double height_mm{121};

  static inline constexpr double dampingCoefficient_N_m_sec{0};

  // Lets add a sanity check
  // hopefully brownie points
  //  W = 1 N*m/s ,
  // P_max = torque * AngularVel_max
  static_assert(maximumPowerConsumption_W >=
                    (outputTorque_Floor_N_m * maximumAngularRate_rad_sec),
                "configuration likely incorrect, not physically possible "
                "as a device");
};

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
    aRetVal =
        std::clamp(rhsInput, (0 - damperConfig.maximumAngularRate_rad_sec),
                   damperConfig.maximumAngularRate_rad_sec);
    return aReturn;
  }
  // protected:

private:
  RateDamperConfiguration damperConfig;
};
