/**
 * \file RateDamper.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
// #include <csignal>
// #include <cstdio>
#include <cstdint>
#include <numbers>

/// CMAKE INCLUDES
// #include "RateDamper/version.h"

/// USER INCLUDES

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
  static inline constexpr double outputTorqueM_ax_N_m_Hz{11};
  static inline constexpr double maximumAngularRate_rev_min{1200};
  static inline constexpr double angularRate_Absolute_Max_rad_sec{
      // Rev/min * min/sec *rad/rev = Rev/sec * rad/rev = rad/sec
      (maximumAngularRate_rev_min * seconds_to_minute) * (2 * pi)};

  static inline constexpr double nominalVoltage_V{28.0};
  static inline constexpr double nomalVoltageVariation_V{3.0};
  static inline constexpr double maximumPowerConsumption_W{15.0};

  // Physical properties
  static inline constexpr double mass_kg{10};
  static inline constexpr double diameter_mm{337};

  //                                        mm*m/mm
  static inline constexpr double diameter_m{diameter_mm * mm_to_m};

  static inline constexpr double height_mm{121};
  //                                        mm*m/mm
  static inline constexpr double height_m { height_mm *mm_to_m }
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

  // protected:

  double control(double rhsAngularRate) {};

  double boundedVoltageValidation(double rhsInput) {
    return minMaxCheck(
        rhsInput,
        (damperConfig.nominalVoltage_V - damperConfig.nomalVoltageVariation_V),
        (damperConfig.nominalVoltage_V + damperConfig.nomalVoltageVariation_V));
  };

  double boundedAngularRateValidation(double rhsInput) {
    return minMaxCheck(rhsInput, (0 - damperConfig.maximumAngularRate_rev_min),
                       damperConfig.maximumAngularRate_rev_min);
  }

private:
  RateDamperConfiguration damperConfig;
  // Helper Function
  inline constexpr double minMaxCheck(double rhsValue, double rhsMinimum,
                                      double rhsMaximum) {
    return (rhsValue < rhsMinimum)   ? rhsMinimum
           : (rhsValue > rhsMaximum) ? rhsMaximum
                                     : rhsValue;
  }
};
