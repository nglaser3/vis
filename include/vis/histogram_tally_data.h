#pragma once

#include <caeror/raja_layouts.h>

#include "vis/indexes.h"
#include "vis/mesh.h"
#include "vis/histogram_tally_state.h"

namespace vis {
template <typename... MeshPhases>
class HistogramTallyData {
 public:
  using TallyLayout = caeror::CaerorLayout<MeshPhases...>;
  using TallyView = caeror::CaerorWritableView<double, TallyLayout>;
  using TallyState = HistogramTallyState<MeshPhases...>;
  using Meshes = camp::tuple<UniformMesh<MeshPhases>...>;

  explicit HistogramTallyData(RAJA::resources::Resource& resource,
                              const UniformMesh<MeshPhases>&... meshes)
      : resource_(resource),
        layout_(meshes.GetNumBins()...),
        meshes_(meshes...) {
    tally_data_ = resource_.allocate<double>(layout_.size());
    Reset();
  }

  HistogramTallyData(const HistogramTallyData&) = delete;
  HistogramTallyData& operator=(const HistogramTallyData&) = delete;
  HistogramTallyData(HistogramTallyData&& other) = delete;
  HistogramTallyData& operator=(HistogramTallyData&& other) = delete;

  ~HistogramTallyData() {
    if (tally_data_) {
      resource_.wait();
      resource_.deallocate(tally_data_);
      tally_data_ = nullptr;
    }
  }

  void Reset() {
    resource_.memset(tally_data_, 0, layout_.size() * sizeof(double));
    resource_.wait();
  }

  RAJA_HOST_DEVICE TallyState GetState() const {
    return TallyState{TallyView(tally_data_, layout_), meshes_};
  }

 private:
  RAJA::resources::Resource& resource_;
  TallyLayout layout_;
  Meshes meshes_;
  double* tally_data_ = nullptr;
};
}  // namespace vis
