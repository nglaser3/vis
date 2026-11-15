# --------------------------------------------------------------------------- #
# Root level CMake file for vis
# --------------------------------------------------------------------------- #

cmake_minimum_required(VERSION 3.24)

project(
  vis
  VERSION 0.0.0
  LANGUAGES CXX C
)
set(BLT_CXX_STD "c++23" CACHE STRING "C++ standard used by BLT")

include(cmake/VisPathVariables.cmake)
include(cmake/VisVariables.cmake)
include(cmake/VisSetupBLT.cmake)

if(VIS_BUILD_TESTS)
  enable_testing()
endif()

include(cmake/VisSetupRAJA.cmake)
include(cmake/VisSetupCaeror.cmake)

add_subdirectory("${VIS_SRC_DIR}")
target_compile_features(vis PRIVATE cxx_std_23)
