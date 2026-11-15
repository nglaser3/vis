#pragma once

#include <caeror/raja_layouts.h>

#include "vis/indexes.h"

namespace vis {
template <typename IndexType>
class UniformMesh {
 public:
  UniformMesh(double min, double max, caeror::CaerorIndexType num_bins)
      : min_(min), max_(max), num_bins_(num_bins) {}

  RAJA_HOST_DEVICE bool InBounds(const double value) const {
    return value >= min_ && value < max_;
  }

  RAJA_HOST_DEVICE IndexType GetIndex(const double value) const {
    auto index = static_cast<caeror::CaerorIndexType>(
      (value - min_) / (max_ - min_) * num_bins_);
    return IndexType{index};
  }

 private:
  double min_, max_;
  caeror::CaerorIndexType num_bins_;
};
}  // namespace vis
