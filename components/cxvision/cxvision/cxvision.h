/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef CXVISION_CXVISION_H_
#define CXVISION_CXVISION_H_

#include "cxvision/graph/graph_manager.h"
#include "cxvision/plugin/plugin.h"
#ifdef CXVISION_USE_PROTOBUF
#include "cxvision/proto/simple_meta.pb.h"
#include "cxvision/proto/voice.pb.h"
#else
#include "cxvision/proto/simple_meta.h"
#include "cxvision/proto/voice.h"
#endif

namespace cx {

  bool Init();

}  // namespace cx

#endif  // CXVISION_CXVISION_H_
