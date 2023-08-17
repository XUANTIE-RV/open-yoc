FILE(GLOB SOURCE_FILES
  ${COMPONENTS_DIR}/alg_dispatch/dispatch_ringbuf.c
  ${COMPONENTS_DIR}/alg_dispatch/dispatch_process.cc
)

include_directories(${COMPONENTS_DIR}/alg_dispatch/include)

add_library(alg_dispatch STATIC ${SOURCE_FILES})
