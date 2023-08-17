/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#ifndef TM_TRACE_H
#define TM_TRACE_H

#include <string>
#ifdef __linux__
#include <tmedia_config.h>
#endif

#ifdef CONFIG_TMEDIA_TRACE
#include <perfetto.h>
#endif

#ifdef _MSC_VER
#define __FUNC_STR__ __FUNCSIG__
#else
#define __FUNC_STR__ __PRETTY_FUNCTION__
#endif

#define CAT_DEBUG "debug"
#define CAT_AIE_IMAGEPROC "ImageProc"

class TMTracer
{
public:
    TMTracer(std::string file = "example.pftrace", uint32_t buf_size = 1024);
    virtual ~TMTracer();

public:
    int Start();
    int Stop();

private:
    std::string mFile;
    uint32_t mBufSize;
#ifdef CONFIG_TMEDIA_TRACE
    std::unique_ptr<perfetto::TracingSession> mSession;
#endif
};

#ifdef CONFIG_TMEDIA_TRACE

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category(CAT_AIE_IMAGEPROC)
        .SetDescription("AIEngine-ImageProc"),
    perfetto::Category(CAT_DEBUG)
        .SetDescription("Verbose events"));

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

#define TRACE_META_PROCESS(process_name)                                      \
{                                                                             \
    perfetto::ProcessTrack process_track = perfetto::ProcessTrack::Current(); \
    perfetto::protos::gen::TrackDescriptor desc = process_track.Serialize();  \
    desc.mutable_process()->set_process_name(process_name);                   \
    perfetto::TrackEvent::SetTrackDescriptor(process_track, desc);            \
}
#define TRACE_META_THREAD(thread_name)                                      \
{                                                                           \
    perfetto::ThreadTrack thread_track = perfetto::ThreadTrack::Current();  \
    perfetto::protos::gen::TrackDescriptor desc = thread_track.Serialize(); \
    desc.mutable_thread()->set_thread_name(thread_name);                    \
    perfetto::TrackEvent::SetTrackDescriptor(thread_track, desc);           \
}

#else

#ifdef TRACE_EVENT_BEGIN
#undef TRACE_EVENT_BEGIN
#endif
#define TRACE_EVENT_BEGIN(category, name, ...)

#ifdef TRACE_EVENT_END
#undef TRACE_EVENT_END
#endif
#define TRACE_EVENT_END(category, ...)

#ifdef TRACE_EVENT
#undef TRACE_EVENT
#endif
#define TRACE_EVENT(category, name, ...)

#ifdef TRACE_COUNTER
#undef TRACE_COUNTER
#endif
#define TRACE_COUNTER(category, name, ...)

#define TRACE_META_PROCESS(process_name)
#define TRACE_META_THREAD(thread_name)
#endif

#endif /* TM_TRACE_H */
