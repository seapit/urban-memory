/**
 * \file PingPongBuffer.hpp
 * \brief A brief description of what this file is.
 */

#pragma once
/// INCLUDES
#include <array>
#include <atomic>
#include <cstddef>

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

    // update index so the next read will always read the latest stored
    // value
    ping_pong.store(currentIndex);
  }

  // returning a reference here wouldn't be good as it could change mid-write,
  // we want consumers of these 'T's to be able to see the genuine last value
  // they should've
  T getLatest() const noexcept { return samples[ping_pong.load()]; }

private:
  // just use an XOR to swap between the buffers
  // not optimizing here for memory order/thread sync
  // could use fetch add and remove need for storing it later... but obliges us
  // to use masking
  inline std::size_t getNextIndex() { return (1 ^ ping_pong.load()); }

  // default initialize the array so any consumer will get valid
  // zero-initialized item
  std::array<T, numberOfBuffers> samples{};

  // this is our index
  std::atomic<std::size_t> ping_pong{0};
};
