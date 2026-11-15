# --------------------------------------------------------------------------- #
# Setup vis executable
# --------------------------------------------------------------------------- #

blt_add_executable(
  NAME vis
  SOURCES
    main.cc
  DEPENDS_ON
    caeror
    RAJA
)
