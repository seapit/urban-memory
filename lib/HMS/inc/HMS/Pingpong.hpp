/**
 * \file HMS.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
#ifndef TM_PING_PONG__
#define TM_PING_PONG__
/// INCLUDES
// #include <csignal>
// #include <cstdio>
#include <array>
#include <atomic>

/// CMAKE INCLUDES
// #include "HMS/version.h"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class HMS
 */
template <class T> class pingPongBase {
  static inline constexpr std::size_t numberOfBuffers{2};

public:
  // must automatically update the index value so we can swap
  // use only two buffers so we have less scope
  // if using multiple buffers, would just ensure power of 2 and use masking and
  // a size_t to not need to keep track of bounds, and overflows are still valid
  // with this
  void update(const T &rhsSensorInput) noexcept {
    const auto currentIndex = getCurrentIndex();
    samples[currentIndex] = rhsSensorInput;
    ping_pong.store(currentIndex);
  }

  T getLatest() noexcept {
    const auto currentIndex = getCurrentIndex();
    return samples[currentIndex];
  }

private:
  // just use an XOR to swap between the buffers
  // not optimizing here for memory order/thread sync
  // could use fetch add and remove need for storing it later... but obliges us
  // to use masking
  inline std::size_t getCurrentIndex() { return (1 ^ ping_pong.load()); }

  std::array<T, numberOfBuffers> samples;
  std::atomic<std::size_t> ping_pong{0};
};

#endif // HMS__
