# --------------------------------------------------------------------------- #
# Setup BLT using RAJA's bundled dependency
# --------------------------------------------------------------------------- #

if(NOT DEFINED BLT_SOURCE_DIR)
  set(BLT_SOURCE_DIR
    "${VIS_TPL_DIR}/raja/blt"
    CACHE PATH "Path to BLT"
  )
endif()

if(NOT EXISTS "${BLT_SOURCE_DIR}/SetupBLT.cmake")
  message(FATAL_ERROR
    "BLT not found: ${BLT_SOURCE_DIR}. Run git submodule update --init --recursive."
  )
endif()

if(ENABLE_OPENMP AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  find_package(OpenMP REQUIRED)
  if(NOT BLT_OPENMP_COMPILE_FLAGS)
    separate_arguments(BLT_OPENMP_COMPILE_FLAGS NATIVE_COMMAND "${OpenMP_CXX_FLAGS}")
    foreach(include_dir IN LISTS OpenMP_CXX_INCLUDE_DIRS)
      list(APPEND BLT_OPENMP_COMPILE_FLAGS "-I${include_dir}")
    endforeach()
  endif()
  if(NOT BLT_OPENMP_LINK_FLAGS)
    set(BLT_OPENMP_LINK_FLAGS "${OpenMP_CXX_LIBRARIES}")
  endif()
endif()

include("${BLT_SOURCE_DIR}/SetupBLT.cmake")
