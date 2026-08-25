/**
 * \file PingPongBuffer.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <array>
#include <atomic>

/// CMAKE INCLUDES
// #include "PingPongBuffer/version.h"

/// USER INCLUDES

/// NAMESPACE

/// DEFINES

/// CODE
/**
 * \class PingPongBuffer
 * \note template obliges that this is defined only in a header
 */
template <class T> class PingPongBuffer {
  static inline constexpr std::size_t numberOfBuffers{2};

public:
  // must automatically update the index value so we can swap
  // use only two buffers so we have less scope
  // if using multiple buffers, would just ensure power of 2 and use masking and
  // a size_t to not need to keep track of bounds, and overflows are still valid
  // with this
  void update(const T &rhsSensorInput) noexcept {
    const auto currentIndex = getNextIndex();
    samples[currentIndex] = std::move(rhsSensorInput);
    ping_pong.store(currentIndex);
  }

  const T &getLatest() noexcept { return samples[ping_pong.load()]; }

private:
  // just use an XOR to swap between the buffers
  // not optimizing here for memory order/thread sync
  // could use fetch add and remove need for storing it later... but obliges us
  // to use masking
  inline std::size_t getNextIndex() { return (1 ^ ping_pong.load()); }

  std::array<T, numberOfBuffers> samples;
  std::atomic<std::size_t> ping_pong{0};
};
