#
# Copyright (C) 2022-2023 Alibaba Group Holding Limited
#

# include
INCLUDE_DIRECTORIES(./include)

#extern include
INCLUDE_DIRECTORIES(../perfetto)

SET(LIBSOURCE
	src/common/src/clock.cpp
	src/common/src/error.cpp
	src/common/src/frame.cpp
	src/common/src/image_info.cpp
	src/common/src/media_info.cpp
	src/common/src/packet.cpp
	src/common/src/property.cpp
	src/common/src/syslog.cpp

	src/memory/src/buffer.c
	src/memory/src/buffer_pool.c
	src/memory/src/default_buffer_allocator.c
	src/memory/src/vdmabuf.c
	src/memory/src/vdmabuf.h

	src/util/src/util_load.cpp
	src/util/src/util_save.cpp
	src/util/src/util_system.cpp
	src/util/src/util_timer.cpp
    src/util/src/util_ringbuffer.cpp

	src/bind/src/event_subscriber.cpp
	src/bind/src/pad.cpp
	src/bind/src/event.cpp
	src/bind/src/pipeline.cpp

	../perfetto/perfetto.cpp
	src/tracer/src/tracer.cpp

	src/profiler/src/profiler.cpp

	src/entity/parser/src/parser.cpp
	src/entity/parser/src/parser_factory.cpp
        
    src/entity/format/src/format_demuxer.cpp
	src/entity/format/src/format_factory.cpp
	src/entity/format/src/format_info.cpp

	src/entity/entity.cpp
	src/entity/codec/src/codec.cpp
	src/entity/codec/src/codec_factory.cpp
	src/entity/codec/src/codec_params.cpp
	src/entity/codec/src/decoder.cpp
	src/entity/codec/src/encoder.cpp

	src/entity/vpm/src/vpm_factory.cpp

	src/entity/vpss/src/vpss.cpp
	src/entity/vpss/src/vpss_factory.cpp
	src/entity/vpss/src/vpss_params.cpp

	src/entity/aiengine/src/aie_factory.cpp
)

IF (NOT CONFIG_TMEDIA_TRACE)
    list(REMOVE_ITEM LIBSOURCE "../perfetto/perfetto.cpp")
ENDIF(NOT CONFIG_TMEDIA_TRACE)

ADD_LIBRARY(tmedia_core SHARED ${LIBSOURCE})

target_link_libraries(tmedia_core dw rt)

INSTALL(DIRECTORY ./include/. DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
INSTALL(TARGETS tmedia_core
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

