# --------------------------------------------------------------------------- #
# Setup tests for vis
# --------------------------------------------------------------------------- #

blt_add_executable(
  NAME vis_tests
  SOURCES
    tally/histogram_tally_state.test.cc
    mesh/uniform_mesh.test.cc
  INCLUDES
    "${VIS_INCLUDE_DIR}"
  DEPENDS_ON
    caeror
    RAJA
    gtest
)

blt_add_test(
  NAME vis_tests
  COMMAND vis_tests
)
