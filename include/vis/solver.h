#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

#include <RAJA/RAJA.hpp>
#include <caeror/caeror_geometry.h>
#include <caeror/caeror_geometry_state.h>

#include "vis/exec_policy.h"
#include "vis/histogram_tally_data.h"
#include "vis/material_bank.h"
#include "vis/particle_bank.h"
#include "vis/particle_stepper.h"
#include "vis/random.h"

namespace vis {

template <double AzimuthalEpsilon>
class Solver {
 public:
  Solver(caeror::CaerorGeometry& geometry_data, MaterialBank& material_bank,
         const ParticleBank& particle_bank)
      : geometry_data_(geometry_data), material_bank_(material_bank),
        particle_bank_(particle_bank) {
    material_bank_.CalculateXS();
  }

  template <typename ExecutionPolicy = VisExecutionPolicy,
            SolverType Method = Milstein, typename... MeshPhases>
  void SolveSDE(RAJA::resources::Resource& resource, double ds_scale,
                HistogramTallyData<MeshPhases...>& tally_data) {
    using Exec = typename ExecutionPolicy::ExecPolicy;
    using Atomic = typename ExecutionPolicy::AtomicPolicy;
    using Resource = typename RAJA::resources::get_resource<Exec>::type;

    const auto geometry = geometry_data_.GetState(resource);
    const auto particle_data = particle_bank_.GetParticleView();
    const auto num_particles = particle_data.get_layout().template get_dim_size<0>();
    const auto transport = material_bank_.GetTransportXSView();
    const auto large_scatter = material_bank_.GetLargeScatterXSView();
    const auto eta = material_bank_.GetEtaView();
    const auto muc = material_bank_.GetMuCView();

    tally_data.Reset();
    const auto tally = tally_data.GetState();

    RAJA::forall<Exec>(resource.get<Resource>(),
      RAJA::TypedRangeSegment<index::PID>(0, num_particles),
      [=] RAJA_HOST_DEVICE (index::PID pid) {
        auto p = ParticleBank::InitParticle(pid, particle_data);
        p.cell = geometry.FindCell(p);
        if (*p.cell == caeror::MAXCaerorIndex) p.alive = false;

        double optical_depth = SampleOpticalDepth(p);

        while (p.alive) {
          double remaining_step_mfp = ds_scale;

          while (remaining_step_mfp > 0.0 && p.alive) {
            const auto material = geometry.material_fills(p.cell);
            const double collision_xs = large_scatter(material);
            const double transport_xs = transport(material);

            const auto boundary = geometry.DistanceToCellBoundary(p);
            const double collision_distance = optical_depth / collision_xs;
            const double distance = fmin(remaining_step_mfp / transport_xs, fmin(boundary.distance, collision_distance));

            const bool collide = collision_distance <= distance;
            const bool cross = !collide && boundary.distance <= distance;

            tally.template ScoreTracklength<Atomic>(p, distance);
            p.Move(distance);

            remaining_step_mfp -= distance * transport_xs;
            optical_depth -= distance * collision_xs;

            if (collide) {
              optical_depth = SampleOpticalDepth(p);
              const double cosine = MaterialBank::SampleScatteringCosine(LCG(p), material, eta, muc);
              const double azimuth = 2.0 * std::numbers::pi * LCG(p);
              ApplyHardCollision(p, cosine, azimuth);
            }

            const SolverData data {
              .ds = distance,
              .transport_xs = transport_xs,
              .epsilon = AzimuthalEpsilon
            };
            ApplySoftCollision<Method>(p, data);

            if (cross) {
              const auto crossing = geometry.CrossSurface(p, boundary.surface);
              p.cell = crossing.cell;
              p.alive = !crossing.leaked;
              p.mu = fmax(-1.0, fmin(p.uz, 1.0));
              p.azi = atan2(p.uy, p.ux);
            }
          }
        }
      });
    resource.wait();
  }

 private:
  static RAJA_HOST_DEVICE double SampleOpticalDepth(Particle& p) {
    return -log(1 - LCG(p));//since lcg is [0, 1)
  }

  static RAJA_HOST_DEVICE void ApplyHardCollision(Particle& p, double mu_s, double azi_s) {
    const double sin_mu_s = sqrt(fmax(0.0, 1.0 - mu_s * mu_s));
    const double sin_mu = sqrt(fmax(0.0, 1.0 - p.mu * p.mu));
    const double cos_azi = cos(p.azi);
    const double sin_azi = sin(p.azi);
    const double cos_azi_s = cos(azi_s);
    const double sin_azi_s = sin(azi_s);

    const double ux = mu_s * p.ux - sin_mu_s * (p.mu * cos_azi * cos_azi_s + sin_azi * sin_azi_s);
    const double uy = mu_s * p.uy + sin_mu_s * (-p.mu * sin_azi * cos_azi_s + cos_azi * sin_azi_s);
    const double mu = mu_s * p.mu + sin_mu_s * sin_mu * cos_azi_s;

    p.UpdateDirection(fmax(-1.0, fmin(mu, 1.0)), atan2(uy, ux));
  }

  template <SolverType Method>
  static RAJA_HOST_DEVICE void ApplySoftCollision(Particle& p, const SolverData& data) {
    const auto normal = NormalDistribution<0.0, 1.0>(p);
    const double mu = SampleNewMu<Method>(camp::get<0>(normal), data, p);
    const double azimuth = SampleNewAzimuth<Method>(camp::get<1>(normal), data, p);
    p.UpdateDirection(mu, azimuth);
  }

  caeror::CaerorGeometry& geometry_data_;
  MaterialBank& material_bank_;
  const ParticleBank& particle_bank_;
};

} // namespace vis
