#include <gtest/gtest.h>

#include <cmath>

#include "vis/material_bank.h"

TEST(MaterialBank, ScatteringCosineBoundsAtTransportLimits) {
  RAJA::resources::Resource resource{RAJA::resources::Host{}};

  vis::MaterialBank bank(resource, 2);

  const caeror::MaterialID m1{0}, m2{1};

  auto eta = bank.GetEtaView();
  auto total = bank.GetTotalXSView();
  auto scatter = bank.GetScatterXSView();

  total(m1) = 100.0;
  scatter(m1) = 100.0;
  eta(m1) = 1e-4;
  total(m2) = 50.0;
  scatter(m2) = 50.0;
  eta(m2) = 1e-2;

  for (double cutoff : {-1.0, 1.0}) {
    auto muc = bank.GetMuCView();
    muc(m1) = cutoff;
    muc(m2) = cutoff;
    bank.CalculateXS();
    SCOPED_TRACE(::testing::Message() << "mu_c=" << cutoff);
    for (auto m : {m1, m2}) {
      EXPECT_DOUBLE_EQ(bank.SampleScatteringCosine(0.0, m, eta, muc), -1.0);
      EXPECT_DOUBLE_EQ(bank.SampleScatteringCosine(1.0, m, eta, muc), cutoff);
    }
  }
}

TEST(MaterialBank, CalculatesCrossSectionsForEachMaterial) {
  RAJA::resources::Resource resource{RAJA::resources::Host{}};

  vis::MaterialBank bank(resource, 2);
  auto total = bank.GetTotalXSView();
  auto scatter = bank.GetScatterXSView();
  auto muc = bank.GetMuCView();
  auto eta = bank.GetEtaView();

  for (unsigned i = 0; i < 2; ++i) {
    const caeror::MaterialID m{i};
    total(m) = 8.0;
    scatter(m) = 4.0 * (i + 1);
    muc(m) = 0.0;
    eta(m) = 0.5;
  }
  bank.CalculateXS();
  auto transport = bank.GetTransportXSView();
  auto large_scatter = bank.GetLargeScatterXSView();
  auto scatter_ratio = bank.GetScatterRatioView();

  for (unsigned i = 0; i < 2; ++i) {
    const caeror::MaterialID m{i};
    const double scale = i + 1;
    EXPECT_DOUBLE_EQ(transport(m), scale * 6.0 * (std::log(2.0) - 0.5));
    EXPECT_DOUBLE_EQ(large_scatter(m), scale);
    EXPECT_DOUBLE_EQ(scatter_ratio(m), scale / 8.0);
  }
}

TEST(MaterialBank, SamplesScatteringCosine) {
  RAJA::resources::Resource resource{RAJA::resources::Host{}};

  vis::MaterialBank bank(resource, 1);
  
  const caeror::MaterialID m{0};
  auto eta = bank.GetEtaView();
  auto muc = bank.GetMuCView();
  eta(m) = 0.5;
  muc(m) = 0.0;

  EXPECT_DOUBLE_EQ(bank.SampleScatteringCosine(0.0, m, eta, muc), -1.0);
  EXPECT_DOUBLE_EQ(bank.SampleScatteringCosine(0.5, m, eta, muc), -0.4);
}