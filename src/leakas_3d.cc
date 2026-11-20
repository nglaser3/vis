#include <print>

#include <RAJA/RAJA.hpp>
#include <RAJA/util/Timer.hpp>

#include <conduit.hpp>
#include <conduit_relay.hpp>

#include <caeror/caeror_geometry.h>
#include <caeror/geometry_constructor.h>

#include "vis/histogram_tally_data.h"
#include "vis/material_bank.h"
#include "vis/mesh.h"
#include "vis/particle_bank.h"
#include "vis/solver.h"

int main() {
  constexpr caeror::CaerorIndexType num_histories = 100'000'000;
  constexpr caeror::CaerorIndexType num_bins = 300;
  constexpr double mu_c = 0.99;
  constexpr double ds_scale = 0.01;
  constexpr double azimuthal_epsilon = 1e-10;
  constexpr double source_z = 0.0;
  constexpr double source_x = 0.0;
  constexpr double source_mu = 1.0;
  constexpr double source_azi = 0.0;

  RAJA::resources::Resource resource{RAJA::resources::Host{}};

  caeror::GeometryConstructor input;
  const auto bottom = input.CreateAxisAlignedPlane(
      caeror::Axis::Z, 0.0, caeror::BoundaryCondition::Vacuum);
  const auto interface = input.CreateAxisAlignedPlane(caeror::Axis::Z, 0.75);
  const auto top = input.CreateAxisAlignedPlane(
      caeror::Axis::Z, 3.0, caeror::BoundaryCondition::Vacuum);
  const auto left = input.CreateAxisAlignedPlane(
    caeror::Axis::X, -10.0, caeror::BoundaryCondition::Vacuum);
  const auto right = input.CreateAxisAlignedPlane(
    caeror::Axis::X, 10.0, caeror::BoundaryCondition::Vacuum);
  const auto front = input.CreateAxisAlignedPlane(
    caeror::Axis::Y, -10.0, caeror::BoundaryCondition::Vacuum);
  const auto back = input.CreateAxisAlignedPlane(
    caeror::Axis::Y, 10.0, caeror::BoundaryCondition::Vacuum);
  auto region1 = +bottom && -interface && +left && -right && +front && -back;
  auto region2 = +interface && -top && +left && -right && +front && -back;
  const caeror::MaterialID material1{0}, material2{1};
  input.CreateCell(region1, material1);
  input.CreateCell(region2, material2);
  caeror::CaerorGeometry geometry(input);

  vis::MaterialBank materials(resource, 2);
  auto total = materials.GetTotalXSView();
  auto scatter = materials.GetScatterXSView();
  auto eta = materials.GetEtaView();
  auto cutoff = materials.GetMuCView();
  total(material1) = 100.0;
  scatter(material1) = 100.0;
  eta(material1) = 1e-4;
  cutoff(material1) = mu_c;
  total(material2) = 50.0;
  scatter(material2) = 50.0;
  eta(material2) = 1e-2;
  cutoff(material2) = mu_c;

  vis::ParticleBank particles(resource, num_histories);
  auto particle_data = particles.GetParticleView();
  for (auto pid : RAJA::TypedRangeSegment<vis::index::PID>(0, num_histories)) {
    particle_data(pid, vis::index::ParticleData::x) = source_x;
    particle_data(pid, vis::index::ParticleData::z) = source_z;
    particle_data(pid, vis::index::ParticleData::mu) = source_mu;
    particle_data(pid, vis::index::ParticleData::azi) = source_azi;
  }

  const vis::UniformMesh<vis::index::X> x_mesh(-10.0, 10.0, num_bins / 6);
  const vis::UniformMesh<vis::index::Y> y_mesh(-10.0, 10.0, num_bins / 6);
  const vis::UniformMesh<vis::index::Z> z_mesh(0.0, 3.0, num_bins);
  vis::HistogramTallyData<vis::index::X, vis::index::Y, vis::index::Z> tally(resource, x_mesh, y_mesh, z_mesh);
  vis::Solver<azimuthal_epsilon> solver(geometry, materials, particles);

  std::print("Running Transport\n");
  RAJA::Timer timer;
  timer.start();
  solver.SolveSDE(resource, ds_scale, tally);
  timer.stop();
  std::print("Finished in {0} seconds\n", timer.elapsed());

  conduit::Node node;
  node["runtime"] = timer.elapsed();
  tally.WriteOutResults(node, num_histories);
  conduit::relay::io::save(node, "outputs/leakas_3d.h5");
}