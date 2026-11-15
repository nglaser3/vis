#include <gtest/gtest.h>

#include <cmath>
#include <limits>

#include "vis/mesh.h"

namespace {

TEST(UniformMesh, MapsCoordinatesToBins) {
  const vis::UniformMesh<vis::index::X> mesh(0.0, 10.0, 4);

  EXPECT_EQ(mesh.GetIndex(0.0), vis::index::X{0});
  EXPECT_EQ(mesh.GetIndex(1.25), vis::index::X{0});
  EXPECT_EQ(mesh.GetIndex(2.5), vis::index::X{1});
  EXPECT_EQ(mesh.GetIndex(5.0), vis::index::X{2});
  EXPECT_EQ(mesh.GetIndex(7.5), vis::index::X{3});
  EXPECT_EQ(mesh.GetIndex(std::nextafter(10.0, 0.0)), vis::index::X{3});
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

}  // namespace
