#pragma once

#include <caeror/data.h>

#include <type_traits>

#include "vis/indexes.h"

namespace vis {
struct Particle : caeror::Particle {
  double mu;
  double azi;
  double energy;
  double time;
  bool alive;

  caeror::CaerorIndexType seed;

  RAJA_HOST_DEVICE void UpdateDirection(double mu_, double azi_) {
    mu = mu_;
    azi = azi_;
    const double sinmu = sqrt(1.0 - mu * mu);
    ux = sinmu * cos(azi);
    uy = sinmu * sin(azi);
    uz = mu;
  }

  template <typename Phase>
  RAJA_HOST_DEVICE double GetCoordinate() const {
    if constexpr (std::is_same_v<Phase, index::X>) {
      return x;
    } else if constexpr (std::is_same_v<Phase, index::Y>) {
      return y;
    } else if constexpr (std::is_same_v<Phase, index::Z>) {
      return z;
    } else if constexpr (std::is_same_v<Phase, index::Mu>) {
      return mu;
    } else if constexpr (std::is_same_v<Phase, index::Azi>) {
      return azi;
    } else {
      static_assert(false);
    }
  }

  RAJA_HOST_DEVICE void Move(double distance) {
    x += ux * distance;
    y += uy * distance;
    z += uz * distance;
  }
};
}  // namespace vis
