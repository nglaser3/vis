#pragma once

#include <RAJA/RAJA.hpp>

namespace vis {

  template <typename Exec, typename Atomic = RAJA::auto_atomic>
  struct _ExecutionPolicy {
    using ExecPolicy = Exec;
    using AtomicPolicy = Atomic;
  };

#if defined(RAJA_ENABLE_OPENMP)
  using VisExecutionPolicy = _ExecutionPolicy<RAJA::omp_parallel_for_exec>;
#else
  using VisExecutionPolicy = _ExecutionPolicy<RAJA::seq_exec>;
#endif
  
} // namespace vis
