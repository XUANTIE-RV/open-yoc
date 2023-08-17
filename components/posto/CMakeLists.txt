FILE(GLOB SOURCE_FILES
  ${COMPONENTS_DIR}/posto/posto/*.cc
  ${COMPONENTS_DIR}/posto/posto/base/*.cc
  ${COMPONENTS_DIR}/posto/posto/discovery/*.cc
  ${COMPONENTS_DIR}/posto/posto/domain/*.cc
  ${COMPONENTS_DIR}/posto/posto/entity/*.cc
  ${COMPONENTS_DIR}/posto/posto/message/*.cc
  ${COMPONENTS_DIR}/posto/posto/participant/*.cc
  ${COMPONENTS_DIR}/posto/posto/profile/*.cc
  ${COMPONENTS_DIR}/posto/posto/scheduler/*.cc
  ${COMPONENTS_DIR}/posto/posto/transport/amp/*.cc
  ${COMPONENTS_DIR}/posto/posto/transport/amp/*.c
  ${COMPONENTS_DIR}/posto/posto/transport/ioblock/*.cc
  ${COMPONENTS_DIR}/posto/posto/transport/reader/*.cc
)

add_definitions(-DCONFIG_COMP_IPC -DCONFIG_IPC_RPMSG)

include_directories(${COMPONENTS_DIR}/posto)

add_library(posto STATIC ${SOURCE_FILES})
