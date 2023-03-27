#pragma once
#include "utils/Functional.h"
#include <string>
#include <stdint.h>
#include <functional>

namespace jqutil_v2 {

void PostOnNamedThread(const std::string &name, JQuick::Closure c,
                         int32_t stackSize=0, int32_t priority=0,
                         uint32_t keepAliveMs=10000);
void PostOnNamedThread(const std::string &name, std::function<void()> c,
                         int32_t stackSize=0, int32_t priority=0,
                         uint32_t keepAliveMs=10000);

}  // namespace jqutil_v2
