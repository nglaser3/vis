# --------------------------------------------------------------------------- #
# Root level CMake file for vis
# --------------------------------------------------------------------------- #

cmake_minimum_required(VERSION 3.26)

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
include(cmake/VisSetupConduit.cmake)

add_subdirectory("${VIS_SRC_DIR}")

if(VIS_BUILD_TESTS)
  add_subdirectory("${VIS_TESTS_DIR}")
endif()
