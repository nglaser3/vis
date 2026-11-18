#pragma once

#include <RAJA/RAJA.hpp>
#include <caeror/raja_layouts.h>

namespace vis::index {
RAJA_INDEX_VALUE_T(X, caeror::CaerorIndexType, "X Index");

RAJA_INDEX_VALUE_T(Y, caeror::CaerorIndexType, "Y Index");

RAJA_INDEX_VALUE_T(Z, caeror::CaerorIndexType, "Z Index");

RAJA_INDEX_VALUE_T(Mu, caeror::CaerorIndexType, "Mu Index");

RAJA_INDEX_VALUE_T(Azi, caeror::CaerorIndexType, "Azimuthal Index");

RAJA_INDEX_VALUE_T(Energy, caeror::CaerorIndexType, "Energy Index");

RAJA_INDEX_VALUE_T(Time, caeror::CaerorIndexType, "Time Index");

RAJA_INDEX_VALUE_T(PID, caeror::CaerorIndexType, "Particle ID");

enum ParticleData : caeror::CaerorIndexType {
  x, y, z, mu, azi, e, t
};
}  // namespace vis::index
