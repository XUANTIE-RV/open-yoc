FILE(GLOB SOURCE_FILES
    ${COMPONENTS_DIR}/aos/src/ringbuffer.c
    ${COMPONENTS_DIR}/aos/src/except.c
    ${COMPONENTS_DIR}/aos/src/adapter/linux/aos_linux.c
    ${COMPONENTS_DIR}/aos/src/adapter/linux/port.c
    ${COMPONENTS_DIR}/aos/src/list.c
)

include_directories(${COMPONENTS_DIR}/aos/include)

add_library(aos STATIC ${SOURCE_FILES})
