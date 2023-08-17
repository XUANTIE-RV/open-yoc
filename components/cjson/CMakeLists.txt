FILE(GLOB SOURCE_FILES
  ${COMPONENTS_DIR}/cjson/src/*.c
)

include_directories(${COMPONENTS_DIR}/cjson/include)

add_library(cjson STATIC ${SOURCE_FILES})
