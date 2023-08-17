FILE(GLOB SOURCE_FILES
    ${COMPONENTS_DIR}/alg_pcm_input/adptor/chip_light/pcm_acquire.c
    ${COMPONENTS_DIR}/alg_pcm_input/*.cc
)

include_directories(${COMPONENTS_DIR}/alg_pcm_input/include)

add_library(alg_pcm_input STATIC ${SOURCE_FILES})
