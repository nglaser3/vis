if(NOT TARGET conduit_relay)
  if(NOT EXISTS "${VIS_TPL_DIR}/conduit/src/CMakeLists.txt")
    message(FATAL_ERROR
      "Conduit was not found. Run git submodule update --init --recursive.")
  endif()

  find_path(HDF5_DIR NAMES include/hdf5.h
    HINTS ${HDF5_ROOT} ENV HDF5_ROOT
    REQUIRED)

  set(CONDUIT_ENABLE_TESTS OFF)
  set(ENABLE_EXAMPLES OFF)
  set(ENABLE_UTILS OFF)
  set(ENABLE_DOCS OFF)
  set(ENABLE_PYTHON OFF)
  set(ENABLE_FORTRAN OFF)
  set(ENABLE_RELAY_WEBSERVER OFF)
  add_subdirectory("${VIS_TPL_DIR}/conduit/src")
endif()
