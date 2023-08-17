# Build a test common function lib
INCLUDE_DIRECTORIES(./include)

set(mpegts_srcs 
    src/bytes_io.cpp
    src/ts_demuxer.cpp
    src/ts_format.cpp
    src/ts_frame.cpp
    src/ts_misc.c
    src/ts_muxer.cpp
)
add_library(mpegts STATIC ${mpegts_srcs})

# Config all parameters for test cases below
#set(linkLibs tmedia_core)

