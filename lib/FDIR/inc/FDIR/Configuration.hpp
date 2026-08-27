/**
 * \file Configuration.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <functional>
#include <memory>
#include <string_view>

/// CMAKE INCLUDES
// #include "FDIR/version.h"

/// USER INCLUDES
#include "RateDamper/RateDamper.hpp"
#include "Telemetry/Telemetry.hpp"

/// NAMESPACE

/// DEFINES
/**
 * \brief
 *
 */
struct configuredAction {
  TelemetryIds id{TelemetryIds::MAX};
  RateDamper *damper{nullptr};

  constexpr bool isConfigured() const noexcept {
    return ((id < TelemetryIds::MAX) && (damper != nullptr));
  }
};

/**
 * \brief Barebons configuration of FDIR
 *
 */
struct fdirConfiguration {
  static constexpr std::size_t maximumNumberOfResponses{3};

  // sentinel terminated, the same way channelConfiguration is
  std::array<configuredAction, maximumNumberOfResponses>
      actionableTelemetries{};

  constexpr std::size_t numberOfConfiguredResponses() const noexcept {

    std::size_t aReturn{0};
    for (const auto &aResponse : actionableTelemetries) {
      if (aResponse.isConfigured() == false) {
        break;
      }
      ++aReturn;
    }
    return aReturn;
  }
};