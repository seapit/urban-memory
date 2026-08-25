/**
 * \file Constants.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES

/// CMAKE INCLUDES
// #include "Common/version.h"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES
// this is ugly but i don't know if there's a ready made library for conversions
inline constexpr std::size_t seconds_per_minute{60}; // units of sec/minute
inline constexpr double seconds_to_minute{
    1.0 / seconds_per_minute}; // units of minute/sec

inline constexpr double pi{std::numbers::pi};

/// CODE
