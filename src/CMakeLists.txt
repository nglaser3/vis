# --------------------------------------------------------------------------- #
# Setup vis executable
# --------------------------------------------------------------------------- #

blt_add_executable(
  NAME vis
  SOURCES
    main.cc
  INCLUDES
    "${VIS_INCLUDE_DIR}"
  DEPENDS_ON
    caeror
    RAJA
)
