# --------------------------------------------------------------------------- #
# Setup global path variables for vis
# --------------------------------------------------------------------------- #

set(VIS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

cmake_path(APPEND VIS_ROOT_DIR "src" OUTPUT_VARIABLE VIS_SRC_DIR)
cmake_path(APPEND VIS_ROOT_DIR "include" OUTPUT_VARIABLE VIS_INCLUDE_DIR)
cmake_path(APPEND VIS_ROOT_DIR "tests" OUTPUT_VARIABLE VIS_TESTS_DIR)
cmake_path(APPEND VIS_ROOT_DIR "tpl" OUTPUT_VARIABLE VIS_TPL_DIR)
