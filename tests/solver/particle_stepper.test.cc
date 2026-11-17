#include <gtest/gtest.h>

#include <numbers>

#include "vis/particle_stepper.h"

TEST(ParticleStepper, MilsteinUpdatesMu) {
  const vis::SolverData data{.ds = 0.25, .transport_xs = 1.0, .epsilon = 0.0};
  vis::Particle p{};

  p.mu = 0.0;
  EXPECT_DOUBLE_EQ(vis::SampleNewMu<vis::Milstein>(1.0, data, p), 0.5);
  EXPECT_DOUBLE_EQ(vis::SampleNewMu<vis::Milstein>(10.0, data, p), 1.0);
  EXPECT_DOUBLE_EQ(vis::SampleNewMu<vis::Milstein>(-10.0, data, p), -1.0);
}

TEST(ParticleStepper, MilsteinUpdatesAzimuth) {
  const vis::SolverData data{.ds = 0.25, .transport_xs = 1.0, .epsilon = 0.0};
  vis::Particle p{};

  p.mu = 0.0;
  p.azi = std::numbers::pi - 0.125;
  EXPECT_DOUBLE_EQ(vis::SampleNewAzimuth<vis::Milstein>(0.5, data, p), -std::numbers::pi + 0.125);

  p.azi = 0.75;
  for (double mu : {-1.0, 1.0}) {
    p.mu = mu;
    EXPECT_DOUBLE_EQ(vis::SampleNewAzimuth<vis::Milstein>(1.0, data, p), p.azi);
  }
}
