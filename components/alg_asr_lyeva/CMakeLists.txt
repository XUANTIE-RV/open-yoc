FILE(GLOB SOURCE_FILES
    ${COMPONENTS_DIR}/alg_asr_lyeva/*.cc
    ${COMPONENTS_DIR}/alg_asr_lyeva/nlu/*.c
)

include_directories(${COMPONENTS_DIR}/alg_asr_lyeva/include)
include_directories(${COMPONENTS_DIR}/alg_asr_lyeva/nlu/include)
include_directories(${COMPONENTS_DIR}/alg_nlu_lyeva/include)

file(GLOB LIBSOURCE_LIBA
    ${COMPONENTS_DIR}/alg_asr_lyeva/libs/chip_light/c906fdv/*.a
    ${COMPONENTS_DIR}/alg_nlu_lyeva/libs/chip_light/c910fdv/*.a
)

message(${LIBSOURCE_LIBA})

list(APPEND LIBS_LIBRARIES
    ${LIBSOURCE_LIBA} 
)

add_library(alg_asr_lyeva STATIC ${SOURCE_FILES} ${LIBSOURCE_LIBA})
