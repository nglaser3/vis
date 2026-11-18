#pragma once

#include <cmath>
#include <numbers>

#include <camp/tuple.hpp>
#include <RAJA/RAJA.hpp>

#include "vis/particle.h"

namespace vis {

  /**
   * Adopted from OpenMC's LCG functions. Specifically:
   * commit: ec3bc73070a624286034e7afd068195d31e55ad7
   */
  RAJA_HOST_DEVICE RAJA_INLINE double LCG(Particle& p) {
    constexpr uint64_t multiplication_factor = 6364136223846793005ull;
    constexpr uint64_t additive_factor = 1442695040888963407ull;
    p.seed = multiplication_factor * p.seed + additive_factor;
    auto scrambled = ((p.seed >> ((p.seed >> 59u) + 5u)) ^ p.seed) * 12605985483714917081ull;
    auto result = (scrambled >> 43u) ^ scrambled;
    return static_cast<double>(result >> 11u) * 0x1p-53;
  }

  template <double a, double b>
  RAJA_HOST_DEVICE RAJA_INLINE double UniformDistribution(Particle& p) {
    return a + (b - a) * LCG(p);
  }

  /**
   * Box-Muller transform, adopted from:
   * https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#C++
   */
  template <double mean, double sigma>
  RAJA_HOST_DEVICE RAJA_INLINE camp::tuple<double, double> NormalDistribution(Particle& p) {
    constexpr double two_pi = 2.0 * std::numbers::pi;

    double u1 = 1.0 - LCG(p);
    double u2 = LCG(p);

    auto mag = sigma * sqrt(-2.0 * log(u1));
    auto z0  = mag * cos(two_pi * u2) + mean;
    auto z1  = mag * sin(two_pi * u2) + mean;

    return camp::tuple<double, double>{z0, z1};
  }
  
} // namespace vis
