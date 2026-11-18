#pragma once

#include <cassert>

#include <RAJA/RAJA.hpp>
#include <caeror/raja_layouts.h>

#include "vis/indexes.h"
#include "vis/particle.h"

namespace vis {
  class ParticleBank {
    public:
    using ParticleLayout = caeror::CaerorLayout<index::PID, index::ParticleData>;
    using ParticleView = caeror::CaerorWritableView<double, ParticleLayout>;

    explicit ParticleBank(RAJA::resources::Resource& resource, caeror::CaerorIndexType num_particles)
    : resource_(resource), layout_(num_particles, 7) {
      assert(num_particles != 0);

      particle_data_ = resource.allocate<double>(num_particles * 7);
      resource_.memset(particle_data_, 0, num_particles * 7 * sizeof(double));
      resource_.wait();
    }

    ~ParticleBank() {
      resource_.wait();
      resource_.deallocate(particle_data_);
    }

    ParticleBank(const ParticleBank&) = delete;
    ParticleBank& operator=(const ParticleBank&) = delete;
    ParticleBank(ParticleBank&& other) = delete;
    ParticleBank& operator=(ParticleBank&& other) = delete;

    ParticleView GetParticleView() const {
      return ParticleView(particle_data_, layout_);
    }

    static RAJA_HOST_DEVICE Particle InitParticle(index::PID pid, const ParticleView& view) {
      Particle p{};
      using vis::index::ParticleData;
      p.x = view(pid, ParticleData::x);
      p.y = view(pid, ParticleData::y);
      p.z = view(pid, ParticleData::z);
      p.mu = view(pid, ParticleData::mu);
      p.azi = view(pid, ParticleData::azi);
      p.energy = view(pid, ParticleData::e);
      p.time = view(pid, ParticleData::t);
      p.UpdateDirection(p.mu, p.azi);
      p.seed = *pid;
      p.alive = true;
      return p;
    }

    private:
    RAJA::resources::Resource& resource_;
    ParticleLayout layout_;
    double* particle_data_ = nullptr;
  };
} // namespace vis
