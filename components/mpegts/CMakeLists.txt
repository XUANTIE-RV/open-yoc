CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
INCLUDE(../../cmake/toolchain.cmake)

# include
INCLUDE_DIRECTORIES(./include)
INCLUDE_DIRECTORIES(../aos_port/include)
INCLUDE_DIRECTORIES(../ulog/include)

ADD_DEFINITIONS(
	-DCONFIG_USERVICE_EVENT_TASK_STACK_SIZE=2048
)

SET(LIBSOURCE 
	src/ts_misc.c
	src/bytes_io.cpp
	src/ts_format.cpp
	src/ts_frame.cpp
	src/ts_packet.cpp
	src/ts_muxer.cpp
	src/ts_demuxer.cpp
)

ADD_LIBRARY(mpegts ${LIBSOURCE})

INSTALL(TARGETS mpegts DESTINATION ${INSTALL_PATH}/lib)
INSTALL(DIRECTORY ./include DESTINATION ${INSTALL_PATH}/include)


