FILE(GLOB SOURCE_FILES
    ${COMPONENTS_DIR}/uservice/src/*.c
)

include_directories(${COMPONENTS_DIR}/uservice/include)

add_library(uservice STATIC ${SOURCE_FILES})

