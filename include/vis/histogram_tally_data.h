#pragma once

#include <caeror/raja_layouts.h>
#include <conduit.hpp>

#include <array>
#include <type_traits>
#include <vector>

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

  void WriteOutResults(conduit::Node& output, caeror::CaerorIndexType num_histories) {
    double* write_out_data = new double[layout_.size()];
    resource_.memcpy(write_out_data, tally_data_, layout_.size() * sizeof(double));
    resource_.wait();

    const double bin_volume = (1.0 * ... * camp::get<UniformMesh<MeshPhases>>(meshes_).GetBinWidth());
    const double normalization = static_cast<double>(num_histories) * bin_volume;
    for (caeror::CaerorIndexType i = 0; i < layout_.size(); ++i) {
      write_out_data[i] /= normalization;
    }

    output["tally/mean"].set(write_out_data, static_cast<conduit::index_t>(layout_.size()));
    delete[] write_out_data;

    (camp::get<UniformMesh<MeshPhases>>(meshes_).WriteOut(output), ...);
  }

 private:
  RAJA::resources::Resource& resource_;
  TallyLayout layout_;
  Meshes meshes_;
  double* tally_data_ = nullptr;
};
}  // namespace vis
