# --------------------------------------------------------------------------- #
# Setup compile variables for vis
# --------------------------------------------------------------------------- #

option(VIS_BUILD_TESTS "Build vis tests" ON)

if(VIS_BUILD_TESTS)
  set(ENABLE_GTEST ON CACHE BOOL "Enable Google Test")
endif()
