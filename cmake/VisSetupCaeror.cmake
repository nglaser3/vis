# --------------------------------------------------------------------------- #
# Setup caeror for vis
# --------------------------------------------------------------------------- #

if(NOT TARGET caeror)
  if(NOT EXISTS "${VIS_TPL_DIR}/caeror/CMakeLists.txt")
    message(FATAL_ERROR
      "caeror was not found. Run git submodule update --init --recursive."
    )
  endif()

  set(CAEROR_BUILD_TESTS OFF CACHE BOOL "Build caeror tests" FORCE)
  add_subdirectory("${VIS_TPL_DIR}/caeror")
endif()
