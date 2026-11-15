# --------------------------------------------------------------------------- #
# Setup BLT using RAJA's bundled dependency
# --------------------------------------------------------------------------- #

if(NOT DEFINED BLT_SOURCE_DIR)
  set(BLT_SOURCE_DIR
    "${VIS_TPL_DIR}/raja/blt"
    CACHE PATH "Path to BLT"
  )
endif()

if(NOT EXISTS "${BLT_SOURCE_DIR}/SetupBLT.cmake")
  message(FATAL_ERROR
    "BLT not found: ${BLT_SOURCE_DIR}. Run git submodule update --init --recursive."
  )
endif()

include("${BLT_SOURCE_DIR}/SetupBLT.cmake")
