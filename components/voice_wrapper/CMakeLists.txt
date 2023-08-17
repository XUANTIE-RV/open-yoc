FILE(GLOB SOURCE_FILES
    ${COMPONENTS_DIR}/voice_wrapper/pcm_ops_reg.c
    ${COMPONENTS_DIR}/voice_wrapper/aui_mic_imp.c
)

add_definitions(-DCONFIG_MIC_RINGBUF_RAME=4096)

include_directories(${COMPONENTS_DIR}/voice_wrapper/include)

add_library(voice_wrapper STATIC ${SOURCE_FILES})
