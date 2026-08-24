/**
 * \file OBC.cpp
 * \brief A brief description of what this file is.
 */

/// INCLUDES
// #include <csignal>
// #include <unistd.h>

/// INCLUDES
#include <chrono>
#include <cstdint>

/// CMAKE INCLUDES
// #include "version.h"

/// USER INCLUDES
// #include "App1/OBC.hpp"
/// NAMESPACE

/// DEFINES

/// GLOBAL VARIABLE SHARING FOR CURRENT SYSTEM TICK

/// CODE
int main() {

  // we'll use steps to test our system - 1000 Hz = 1ms tick rate
  // most ambitious linux tick rate
  // not quite sure how
  // static constexpr std::uint16_t systemTickRate_Hz{1000};

  // duration of sim
  // static constexpr std::chrono::seconds simulationDuration{30};
  // number of steps
  //                                            Convert from seconds -> msecs ->
  //                                            get the count
  // static constexpr std::uint16_t numberOfSteps{
  //     std::chrono::duration_cast<std::chrono::milliseconds>(simulationDuration)
  //         .count()};

  // static constexpr std::uint8_t sensor_A_PollRate_Hz{10};
  // static constexpr std::uint8_t sensor_B_PollRate_Hz{50};
  // static constexpr std::uint8_t sensor_C_PollRate_Hz{1};

  while (true) {
    // sleep(10);
  }
  return 0;
}