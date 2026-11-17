#pragma once

#include <numbers>

#include <RAJA/RAJA.hpp>

#include "vis/particle.h"

namespace vis {

  enum SolverType {
    Milstein
  };

  struct SolverData {
    double ds;
    double transport_xs;
    double epsilon;
  };

  template <SolverType>
  RAJA_HOST_DEVICE double SampleNewMu(const double xi, const SolverData& data, const Particle& p);

  template <>
  RAJA_HOST_DEVICE double SampleNewMu<SolverType::Milstein>(const double xi, const SolverData& data, const Particle& p) {
    double dw = sqrt(data.ds) * xi;
    double a = -data.transport_xs * p.mu;
    double b = sqrt(data.transport_xs * (1- p.mu * p.mu));
    double bdb = -data.transport_xs * p.mu;
    const double mn = p.mu + a*data.ds + b*dw + 0.5*bdb*(dw*dw - data.ds);
    return fmax(-1.0, fmin(mn, 1.0));
  }

  template <SolverType>
  RAJA_HOST_DEVICE double SampleNewAzimuth(const double xi, const SolverData& data, const Particle& p);

  template <>
  RAJA_HOST_DEVICE double SampleNewAzimuth<Milstein>(const double xi, const SolverData& data, const Particle& p) {
    if (fabs(p.mu) >= 1 - data.epsilon) return p.azi;
    const double azin = p.azi + sqrt(data.transport_xs / (1 - p.mu * p.mu)) * sqrt(data.ds) * xi;
    return remainder(azin, 2 * std::numbers::pi);
  }

} // namespace vis
