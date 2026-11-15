#include <gtest/gtest.h>

#include "vis/histogram_tally_data.h"

TEST(HistogramTallyState, AccumulatesTracklength) {
  RAJA::resources::Resource resource{RAJA::resources::Host{}};

  const vis::UniformMesh<vis::index::X> x_mesh(0.0, 2.0, 2);
  const vis::UniformMesh<vis::index::Y> y_mesh(0.0, 2.0, 2);
  vis::HistogramTallyData<vis::index::X, vis::index::Y> tally(resource, x_mesh, y_mesh);
  const auto state = tally.GetState();

  vis::Particle p{};
  p.x = 1.5;
  p.y = 1.5;

  RAJA::forall<RAJA::seq_exec>(RAJA::RangeSegment(0, 4), [=](int) {
    state.ScoreTracklength<RAJA::seq_atomic>(p, 2.0);
  });

  EXPECT_DOUBLE_EQ(state.values(vis::index::X{0}, vis::index::Y{0}), 0.0);
  EXPECT_DOUBLE_EQ(state.values(vis::index::X{0}, vis::index::Y{1}), 0.0);
  EXPECT_DOUBLE_EQ(state.values(vis::index::X{1}, vis::index::Y{0}), 0.0);
  EXPECT_DOUBLE_EQ(state.values(vis::index::X{1}, vis::index::Y{1}), 8.0);
}

TEST(HistogramTallyState, SplitsTrackAcrossBins) {
  RAJA::resources::Resource resource{RAJA::resources::Host{}};

  const vis::UniformMesh<vis::index::X> mesh(0.0, 2.0, 2);
  vis::HistogramTallyData<vis::index::X> tally(resource, mesh);
  const auto state = tally.GetState();

  vis::Particle p{};
  p.x = 0.75;
  p.ux = 1.0;
  state.ScoreTracklength<RAJA::seq_atomic>(p, 1.0);

  EXPECT_DOUBLE_EQ(state.values(vis::index::X{0}), 0.25);
  EXPECT_DOUBLE_EQ(state.values(vis::index::X{1}), 0.75);
}

TEST(HistogramTallyState, ScoresOnlyInsideMesh) {
  RAJA::resources::Resource resource{RAJA::resources::Host{}};

  const vis::UniformMesh<vis::index::X> mesh(0.0, 2.0, 1);
  vis::HistogramTallyData<vis::index::X> tally(resource, mesh);
  const auto state = tally.GetState();

  vis::Particle p{};
  p.x = -1.0;
  p.ux = 1.0;
  state.ScoreTracklength<RAJA::seq_atomic>(p, 4.0);

  EXPECT_DOUBLE_EQ(state.values(vis::index::X{0}), 2.0);
}
