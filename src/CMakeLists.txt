# --------------------------------------------------------------------------- #
# Setup vis executable
# --------------------------------------------------------------------------- #

blt_add_executable(
  NAME vis-leakas
  SOURCES
    leakas.cc
  INCLUDES
    "${VIS_INCLUDE_DIR}"
  DEPENDS_ON
    caeror
    RAJA
    conduit_relay
)
