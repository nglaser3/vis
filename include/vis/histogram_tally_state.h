#pragma once

#include <caeror/raja_layouts.h>

#include <cmath>
#include <type_traits>

#include "vis/mesh.h"
#include "vis/particle.h"

namespace vis {
template <typename... MeshPhases>
struct HistogramTallyState {
  using TallyLayout = caeror::CaerorLayout<MeshPhases...>;
  using TallyView = caeror::CaerorWritableView<double, TallyLayout>;
  using Meshes = camp::tuple<UniformMesh<MeshPhases>...>;

  TallyView values;
  Meshes meshes;

  template <typename AtomicPolicy>
  RAJA_HOST_DEVICE void ScoreTracklength(const Particle& p,
                                         double exit_dist) const {
    double dist_traveled = 0.0;
    if (!(ClipTrackHistoryToMesh<MeshPhases>(p, dist_traveled, exit_dist) && ...))
      return;

    camp::tuple<MeshPhases...> bins{BeginningBin<MeshPhases>(p, dist_traveled)...};
    while (dist_traveled < exit_dist) {
      double bin_exit_dist = exit_dist;
      ((bin_exit_dist = fmin(bin_exit_dist, BinExitDistance(p, camp::get<MeshPhases>(bins)))), ...);
      if (bin_exit_dist > dist_traveled) {
        RAJA::atomicAdd<AtomicPolicy>(&values(camp::get<MeshPhases>(bins)...),
                                      bin_exit_dist - dist_traveled);
        dist_traveled = bin_exit_dist;
      }
      if (dist_traveled == exit_dist) return;
      (CrossIntoNextBin(p, bin_exit_dist, camp::get<MeshPhases>(bins)), ...);
    }
  }

 private:
  template <typename Phase>
  RAJA_HOST_DEVICE static double GetDirection(const Particle& p) {
    if constexpr (std::is_same_v<Phase, index::X>)
      return p.ux;
    else if constexpr (std::is_same_v<Phase, index::Y>)
      return p.uy;
    else if constexpr (std::is_same_v<Phase, index::Z>)
      return p.uz;
    else
      return 0.0;
  }

  template <typename Phase>
  RAJA_HOST_DEVICE bool ClipTrackHistoryToMesh(const Particle& p, double& dist_traveled, double& exit_dist) const {
    const auto& mesh = camp::get<UniformMesh<Phase>>(meshes);
    const double coordinate = p.GetCoordinate<Phase>();
    const double update = GetDirection<Phase>(p);

    if (update == 0.0) return mesh.InBounds(coordinate);

    const double dist_to_min = (mesh.GetMin() - coordinate) / update;
    const double dist_to_max = (mesh.GetMax() - coordinate) / update;

    dist_traveled = fmax(dist_traveled, fmin(dist_to_min, dist_to_max));
    exit_dist = fmin(exit_dist, fmax(dist_to_min, dist_to_max));
    return dist_traveled < exit_dist;
  }

  template <typename Phase>
  RAJA_HOST_DEVICE Phase BeginningBin(const Particle& p, double dist_traveled) const {
    const auto& mesh = camp::get<UniformMesh<Phase>>(meshes);
    const double direction = GetDirection<Phase>(p);
    const double coordinate = p.GetCoordinate<Phase>() + dist_traveled * direction;
    return mesh.GetNextBin(fmin(mesh.GetMax(), fmax(mesh.GetMin(), coordinate)),
                           direction);
  }

  template <typename Phase>
  RAJA_HOST_DEVICE double BinExitDistance(const Particle& p, Phase bin) const {
    const auto& mesh = camp::get<UniformMesh<Phase>>(meshes);
    return mesh.GetExitDistance(bin, p.GetCoordinate<Phase>(),
                                GetDirection<Phase>(p));
  }

  template <typename Phase>
  RAJA_HOST_DEVICE void CrossIntoNextBin(const Particle& p, double dist_traveled,
                                          Phase& bin) const {
    if (BinExitDistance(p, bin) <= dist_traveled) {
      bin = Phase{GetDirection<Phase>(p) > 0.0 ? *bin + 1 : *bin - 1};
    }
  }
};
}  // namespace vis
