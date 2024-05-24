#
# Copyright (C) 2022 Alibaba Group Holding Limited
#

project(tmedia_backend_seno LANGUAGES C CXX)
# include
INCLUDE_DIRECTORIES(./include)
INCLUDE_DIRECTORIES(./mpegts/include)

#extern include
INCLUDE_DIRECTORIES(../csinn/include)
INCLUDE_DIRECTORIES(../perfetto)
INCLUDE_DIRECTORIES(../tmedia_core/include)

add_subdirectory(mpegts)

SET(LIBSOURCE 
	src/init.cpp

	src/aiengine/fce_sim.cpp
	src/aiengine/image_proc_opencv.cpp
	src/aiengine/osd_opencv.cpp
	src/aiengine/osd_seno.cpp
	src/aiengine/interpreter_csinn.cpp
	src/aiengine/interpreter_llm.cpp
	src/aiengine/interpreter_onnx.cpp

	src/parser/bs_read.cpp
	src/parser/parser_h264_seno.cpp
	src/format/ts_demuxer_seno.cpp
	src/format/ts_muxer_seno.cpp
	src/format/camera_opencv.cpp
)

link_directories(mpegts)
link_libraries(tmedia_core mpegts)

if (PLATFORM_X86_64)
	link_directories(../csinn/lib/x86_64/)
elseif (PLATFORM_LIGHT)
	link_directories(../csinn/lib/light/)
endif()

ADD_LIBRARY(tmedia_backend_seno SHARED ${LIBSOURCE})

if (PLATFORM_X86_64)
	target_link_libraries(tmedia_backend_seno shl_pnna_x86 dl)
elseif (PLATFORM_LIGHT)
	target_link_libraries(tmedia_backend_seno shl dl onnxruntime -fopenmp)
endif()

if (CONFIG_TMEDIA_SUPPORT_LIB_OPENCV)
	find_package(OpenCV REQUIRED)
	target_link_libraries(tmedia_backend_seno ${OpenCV_LIBRARIES})
endif()

INSTALL(DIRECTORY ./include/. DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
INSTALL(TARGETS tmedia_backend_seno  
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
