#pragma once
#include <string>
#include <vector>
#include "quickjs/quickjs.h"

// some serialize functions with signal C++ types to JSValue types
namespace jqutil_v2 {

template<typename T>
class JQSerializer {
public:
    static JSValue serialize(JSContext *ctx, T x);
    static bool deserialize(JSContext *ctx, JSValueConst v, T *o);
};

// std::string
template<> inline JSValue JQSerializer<std::string>::serialize(JSContext *ctx, std::string x)
{ return JS_NewStringLen(ctx, x.c_str(), x.size()); }
template<> inline bool JQSerializer<std::string>::deserialize(JSContext *ctx, JSValueConst v, std::string *o)
{
    const char* str;
    size_t len;
    if ((str = JS_ToCStringLen(ctx, &len, v)) == NULL) return false;
    o->assign(str, len);
    return true;
}

// int32_t
template<> inline JSValue JQSerializer<int32_t>::serialize(JSContext *ctx, int32_t x)
{ return JS_NewInt32(ctx, x); }
template<> inline bool JQSerializer<int32_t>::deserialize(JSContext *ctx, JSValueConst v, int32_t *o)
{ return JS_ToInt32(ctx, o, v) == 0;  }

// int64_t
// NOTE: same as long long
template<> inline JSValue JQSerializer<int64_t>::serialize(JSContext *ctx, int64_t x)
{ return JS_NewInt64(ctx, x); }
template<> inline bool JQSerializer<int64_t>::deserialize(JSContext *ctx, JSValueConst v, int64_t *o)
{ return JS_ToInt64(ctx, o, v) == 0;  }

// double
template<> inline JSValue JQSerializer<double>::serialize(JSContext *ctx, double x)
{ return JS_NewFloat64(ctx, x); }
template<> inline bool JQSerializer<double>::deserialize(JSContext *ctx, JSValueConst v, double *o)
{ return JS_ToFloat64(ctx, o, v) == 0;  }

// double
template<> inline JSValue JQSerializer<float>::serialize(JSContext *ctx, float x)
{ return JS_NewFloat64(ctx, x); }
template<> inline bool JQSerializer<float>::deserialize(JSContext *ctx, JSValueConst v, float *o)
{
    double tmp;
    bool r = JS_ToFloat64(ctx, &tmp, v) == 0;
    if (r && o) *o = tmp;
    return r;
}

// bool
template<> inline JSValue JQSerializer<bool>::serialize(JSContext *ctx, bool x)
{ return JS_NewBool(ctx, x); }
template<> inline bool JQSerializer<bool>::deserialize(JSContext *ctx, JSValueConst v, bool *o)
{
    int ret;
    if ((ret = JS_ToBool(ctx, v)) == -1) {
        return false;
    }
    *o = ret == 1; /*TRUE*/
    return true;
}

template <typename T> void JQSerializeEach(JSContext *ctx, T && t, std::vector<JSValue> &arr)
{
    arr.push_back(JQSerializer<T>::serialize(ctx, t));
}

}  // namespace jqutil_v2