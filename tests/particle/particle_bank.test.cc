#include <gtest/gtest.h>

#include "vis/particle_bank.h"

TEST(ParticleBank, InitializesParticleFromView) {
  RAJA::resources::Resource resource{RAJA::resources::Host{}};
  vis::ParticleBank bank(resource, 2);
  auto view = bank.GetParticleView();
  const vis::index::PID pid{1};
  using vis::index::ParticleData;

  view(pid, ParticleData::x) = 1.0;
  view(pid, ParticleData::y) = 2.0;
  view(pid, ParticleData::z) = 3.0;
  view(pid, ParticleData::mu) = 0.6;
  view(pid, ParticleData::azi) = 0.0;
  view(pid, ParticleData::e) = 4.0;
  view(pid, ParticleData::t) = 5.0;

  const auto p = vis::ParticleBank::InitParticle(pid, view);

  EXPECT_DOUBLE_EQ(p.x, 1.0);
  EXPECT_DOUBLE_EQ(p.y, 2.0);
  EXPECT_DOUBLE_EQ(p.z, 3.0);
  EXPECT_DOUBLE_EQ(p.mu, 0.6);
  EXPECT_DOUBLE_EQ(p.azi, 0.0);
  EXPECT_DOUBLE_EQ(p.energy, 4.0);
  EXPECT_DOUBLE_EQ(p.time, 5.0);
  EXPECT_DOUBLE_EQ(p.ux, 0.8);
  EXPECT_DOUBLE_EQ(p.uy, 0.0);
  EXPECT_DOUBLE_EQ(p.uz, 0.6);
  EXPECT_EQ(p.seed, 1u);
}
