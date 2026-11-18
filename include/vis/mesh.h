#pragma once

#include <caeror/raja_layouts.h>

#include <cmath>
#include <limits>
#include <type_traits>

#include "vis/indexes.h"

namespace vis {
template <typename IndexType>
class UniformMesh {
 public:
  static constexpr bool IsSpatial = std::is_same_v<IndexType, index::X> ||
                                    std::is_same_v<IndexType, index::Y> ||
                                    std::is_same_v<IndexType, index::Z>;
  UniformMesh(double min, double max, caeror::CaerorIndexType num_bins)
      : min_(min), max_(max), num_bins_(num_bins) {}

  RAJA_HOST_DEVICE caeror::CaerorIndexType GetNumBins() const {
    return num_bins_;
  }

  RAJA_HOST_DEVICE double GetMin() const { return min_; }
  RAJA_HOST_DEVICE double GetMax() const { return max_; }
  RAJA_HOST_DEVICE double GetBinWidth() const {
    return (max_ - min_) / num_bins_;
  }

  RAJA_HOST_DEVICE bool InBounds(const double value) const {
    return value >= min_ && value <= max_;
  }

  RAJA_HOST_DEVICE IndexType GetNextBin(double start, double update) const {
    if constexpr (!IsSpatial) update = 0.0;

    const double relative_location = (start - min_) / GetBinWidth();
    const double bin = update < 0.0 ? ceil(relative_location) - 1.0 : floor(relative_location);

    const auto bin_index = static_cast<caeror::CaerorIndexType>(bin);
    return IndexType{bin_index < num_bins_ ? bin_index : num_bins_ - 1};
  }

  RAJA_HOST_DEVICE double GetExitDistance(IndexType bin, double start,
                                          double update) const {
    if constexpr (!IsSpatial) return std::numeric_limits<double>::infinity();
    if (update == 0.0) return std::numeric_limits<double>::infinity();

    const auto bin_edge = *bin + (update > 0.0 ? 1 : 0);
    const double edge_location =
        bin_edge == num_bins_ ? max_ : static_cast<double>(bin_edge) * GetBinWidth() + min_;
    return (edge_location - start) / update;
  }

 private:
  double min_, max_;
  caeror::CaerorIndexType num_bins_;
};
}  // namespace vis
