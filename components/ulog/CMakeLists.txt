FILE(GLOB SOURCE_FILES
    ${COMPONENTS_DIR}/ulog/src/ulog_linux.c
)

include_directories(${COMPONENTS_DIR}/ulog/include)

add_library(ulog STATIC ${SOURCE_FILES})