/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <fstream>
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/tracer/tracer.h>

TMTracer::TMTracer(std::string file, uint32_t buf_size)
{
    mFile = file;
    mBufSize = buf_size;
}

TMTracer::~TMTracer()
{

}

#ifdef CONFIG_TMEDIA_TRACE
int TMTracer::Start()
{
    perfetto::TracingInitArgs args;
    args.backends = perfetto::kInProcessBackend;
    perfetto::Tracing::Initialize(args);
    perfetto::TrackEvent::Register();
    // perfetto::ConsoleInterceptor::Register();

    perfetto::TraceConfig cfg;
    cfg.add_buffers()->set_size_kb(mBufSize);
    auto* ds_cfg = cfg.add_data_sources()->mutable_config();
    ds_cfg->set_name("track_event");
    // ds_cfg->mutable_interceptor_config()->set_name("console");

    mSession = perfetto::Tracing::NewTrace();
    mSession->Setup(cfg);
    mSession->StartBlocking();

    return TMResult::TM_OK;
}
int TMTracer::Stop()
{
    perfetto::TrackEvent::Flush();

    mSession->StopBlocking();
    std::vector<char> trace_data(mSession->ReadTraceBlocking());

    std::ofstream output;
    output.open(mFile.c_str(), std::ios::out | std::ios::binary);
    output.write(&trace_data[0], std::streamsize(trace_data.size()));
    output.close();

    return TMResult::TM_OK;
}
#else
int TMTracer::Start()
{
    return TMResult::TM_NOT_SUPPORT;;
}
int TMTracer::Stop()
{
    return TMResult::TM_NOT_SUPPORT;;
}
#endif