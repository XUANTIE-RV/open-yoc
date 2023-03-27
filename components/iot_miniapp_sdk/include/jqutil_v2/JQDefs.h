#pragma once
#include "utils/REF.h"

namespace jqutil_v2 {

template <typename T>
using JQRef = JQuick::sp<T>;
template <typename T>
using JQWRef = JQuick::wp<T>;

class JQFunctionInfo;
class JQAsyncInfo;

class JQFunctionTemplate;
class JQObjectProperty;
class JQObjectTemplate;
class JQBaseObject;
class JQAsyncExecutor;
class JQAsyncSchedule;

class JQModuleEnv;
using JQModuleEnvRef = JQRef<JQModuleEnv>;

class JQObjectTemplate;
using JQObjectTemplateRef = JQRef< JQObjectTemplate >;

class JQFunctionTemplate;
using JQFunctionTemplateRef = JQRef< JQFunctionTemplate >;

class JQObjectInternalHolder;
class JQObjectSignalRegister;

}  // namespace jqutil_v2