#include <gtest/gtest.h>

#include <cmath>
#include <limits>

#include "vis/mesh.h"

TEST(UniformMesh, MapsCoordinatesToBins) {
  const vis::UniformMesh<vis::index::X> mesh(0.0, 10.0, 4);

  EXPECT_EQ(mesh.GetNextBin(-1.0, 0.0), vis::index::X{0});
  EXPECT_EQ(mesh.GetNextBin(0.0, 0.0), vis::index::X{0});
  EXPECT_EQ(mesh.GetNextBin(1.25, 0.0), vis::index::X{0});
  EXPECT_EQ(mesh.GetNextBin(2.5, 0.0), vis::index::X{1});
  EXPECT_EQ(mesh.GetNextBin(10.0, 0.0), vis::index::X{3});
  EXPECT_EQ(mesh.GetNextBin(10.1, 0.0), vis::index::X{3});
}

TEST(UniformMesh, BoundsIncludeMinimumAndExcludeMaximum) {
  const vis::UniformMesh<vis::index::X> mesh(0.0, 10.0, 4);

  EXPECT_TRUE(mesh.InBounds(0.0));
  EXPECT_TRUE(mesh.InBounds(2.0));
  EXPECT_FALSE(mesh.InBounds(10.0));
  EXPECT_FALSE(mesh.InBounds(11.0));
}

TEST(UniformMesh, NonFiniteCoordinatesAreOutOfBounds) {
  const vis::UniformMesh<vis::index::X> mesh(0.0, 10.0, 4);

  EXPECT_FALSE(mesh.InBounds(std::numeric_limits<double>::infinity()));
  EXPECT_FALSE(mesh.InBounds(-std::numeric_limits<double>::infinity()));
  EXPECT_FALSE(mesh.InBounds(std::numeric_limits<double>::quiet_NaN()));
}

TEST(UniformMesh, SelectsBinInDirectionOfTravel) {
  const vis::UniformMesh<vis::index::X> mesh(0.0, 3.0, 3);
  EXPECT_EQ(mesh.GetNextBin(1.0, 1.0), vis::index::X{1});
  EXPECT_EQ(mesh.GetNextBin(1.0, -1.0), vis::index::X{0});
  EXPECT_DOUBLE_EQ(mesh.GetExitDistance(vis::index::X{1}, 1.0, 0.5), 2.0);
  EXPECT_TRUE(std::isinf(mesh.GetExitDistance(vis::index::X{1}, 1.0, 0.0)));
  EXPECT_DOUBLE_EQ(mesh.GetExitDistance(vis::index::X{0}, 1.0, -0.5), 2.0);
}

TEST(UniformMesh, NonspatialPhasesIgnoreUpdate) {
  const vis::UniformMesh<vis::index::Energy> energy(0.0, 10.0, 2);
  EXPECT_EQ(energy.GetNextBin(5.0, -1.0), vis::index::Energy{1});
  EXPECT_EQ(energy.GetNextBin(5.0, 1.0), vis::index::Energy{1});
  EXPECT_TRUE(std::isinf(energy.GetExitDistance(vis::index::Energy{1}, 5.0, 1.0)));
}
