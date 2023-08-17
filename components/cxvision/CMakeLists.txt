FILE(GLOB SOURCE_FILES
  ${COMPONENTS_DIR}/cxvision/cxvision/*.cc
  ${COMPONENTS_DIR}/cxvision/cxvision/base/*.cc
  ${COMPONENTS_DIR}/cxvision/cxvision/graph/*.cc
  ${COMPONENTS_DIR}/cxvision/cxvision/plugin/*.cc
)

include_directories(${COMPONENTS_DIR}/cxvision)

add_library(cxvision STATIC ${SOURCE_FILES})
