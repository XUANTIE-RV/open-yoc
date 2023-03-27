#pragma once
#include "quickjs/quickjs.h"
#include "jqutil_v2/JQSerializer.h"
#include "jqutil_v2/jqmisc.h"
#include <assert.h>
#include <vector>
#include <string>
#include <map>

namespace jqutil_v2 {

class JQValue {
public:
    JQValue(JSContext *ctx, JSValueConst value): _ctx(ctx), _value(value), _scope(false) {}
     ~JQValue() {
         if (_scope) {
             JS_FreeValue(_ctx, _value);
         }
         _ctx = nullptr;
         _value = JS_UNDEFINED;
     }
     JQValue(const JQValue &rval)
     {
         assert(rval._ctx == _ctx);
         this->~JQValue();
         _ctx = rval._ctx;
         _value = rval._value;
         if (_scope) {
             JS_DupValue(_ctx, _value);
         }
     }
    inline JSValueConst value() { return _value; }
    inline void setValue(JSValueConst val)
    { if (_scope) jq_set_value(_ctx, &_value, val);
        else _value = val; }
    inline bool IsEmpty() { return JS_IsUndefined(_value); }

    inline bool isArray() { return JS_IsArray(_ctx, _value); }
    inline bool isNumber() { return JS_IsNumber(_value); }
    inline bool isString() { return JS_IsString(_value); }
    inline bool isBool() { return JS_IsBool(_value); }
    inline bool isObject() { return JS_IsObject(_value); }
    inline bool isFunction() { return JS_IsFunction(_ctx, _value); }

    // some specify type check
    inline bool isInt() { return JS_VALUE_GET_TAG(_value) == JS_TAG_INT; }
    inline bool isDouble() { return JS_TAG_IS_FLOAT64(JS_VALUE_GET_TAG(_value)); }

    const char* getTypeof();

protected:
    inline void _setScope(bool scope) { _scope = scope; }

protected:
    JSContext *_ctx;
    JSValue _value;
    bool _scope;
};

class JQString: public JQValue {
public:
    JQString(JSContext *ctx, JSValueConst value=JS_UNDEFINED)
        :JQValue(ctx, value), _ptr(NULL), _converted(false)
    {}

    ~JQString() { if (_ptr) { JS_FreeCString(_ctx, _ptr); } }

    const char* get() { _convert(); return _ptr; }
    inline std::string getString() { _convert(); return _ptr ? _ptr : ""; }
    size_t len() const { return _len; }

private:
    inline void _convert()
    {
        if (!_converted) {
            _converted = true;
            _ptr = JS_ToCStringLen(_ctx, &_len, _value);
        }
    }

private:
    size_t _len;
    const char* _ptr;
    bool _converted;
};

class JQSymbol: public JQValue {
public:
    JQSymbol(JSContext *ctx, JSValueConst value=JS_UNDEFINED)
    :JQValue(ctx, value) {}
};

class JQNumber: public JQValue {
public:
    JQNumber(JSContext *ctx, JSValueConst value=JS_UNDEFINED)
        :JQValue(ctx, value) {}
    inline int32_t getInt32() { int32_t val = 0; JS_ToInt32(_ctx, &val, _value); return val; }
    inline uint32_t getUint32() { uint32_t val = 0; JS_ToUint32(_ctx, &val, _value); return val; }
    inline int64_t getInt64() { int64_t val = 0; JS_ToInt64(_ctx, &val, _value); return val; }
    inline double getDouble() { double val = 0; JS_ToFloat64(_ctx, &val, _value); return val; }
};

class JQBool: public JQValue {
public:
    JQBool(JSContext *ctx, JSValueConst value=JS_UNDEFINED)
        :JQValue(ctx, value) {}
    inline bool getBool() { return JS_ToBool(_ctx, _value) == 1; }
};

class JQFunction: public JQValue {
public:
    JQFunction(JSContext *ctx, JSValueConst value=JS_UNDEFINED)
            :JQValue(ctx, value) {}
};

class JQObject: public JQValue {
public:
    JQObject(JSContext *ctx, JSValueConst value=JS_UNDEFINED)
            :JQValue(ctx, value), _extracted(false) {}
    virtual ~JQObject();

    inline const std::map<std::string, JSValueConst>& keyValueMap()
    { if (!_extracted) { _tryExtract(); _extracted = true; }
        return _keyValueMap; }

    bool getBool(const std::string &prop);
    inline int32_t getInt32(const std::string &prop)
    { int32_t val = 0; JS_ToInt32(_ctx, &val, getValue(prop)); return val; }
    inline uint32_t getUint32(const std::string &prop)
    { uint32_t val = 0; JS_ToUint32(_ctx, &val, getValue(prop)); return val; }
    inline int64_t getInt64(const std::string &prop)
    { int64_t val = 0; JS_ToInt64(_ctx, &val, getValue(prop)); return val; }
    inline double getDouble(const std::string &prop)
    { double val = 0; JS_ToFloat64(_ctx, &val, getValue(prop)); return val; }

    std::string getString(const std::string &prop);

    JSValueConst getValue(const std::string &prop);
    inline JSValue getDupValue(const std::string &prop)
    { return JS_DupValue(_ctx, getValue(prop)); }

    void clearCache() { _clearKeyValueMap(); _extracted = false; }

private:
    bool _tryExtract();
    void _clearKeyValueMap();

private:
    std::map<std::string, JSValue> _keyValueMap;
    bool _extracted;
};

class JQArray: public JQValue {
public:
    JQArray(JSContext *ctx, JSValueConst value=JS_UNDEFINED)
        :JQValue(ctx, value) {}
    uint32_t length() {
        uint32_t len = 0;
        JSValue len0 = JS_GetPropertyStr(_ctx, _value, "length");
        JS_ToUint32(_ctx, &len, len0);
        JS_FreeValue(_ctx, len0);
        return len;
    }
    template<typename T>
    inline bool getItem(uint32_t idx, T *o)
    {
        JSValueConst val = at(idx);
        return JQSerializer<T>::deserialize(_ctx, val, o);
    }

    inline JSValueConst at(uint32_t idx)
    {
        JSValue val = JS_GetPropertyUint32(_ctx, _value, idx);
        JS_FreeValue(_ctx, val);
        return val;
    }
    inline void toStringVector(std::vector<std::string > &result)
    {
        JSValueConst item;
        const char* cstr;
        for (unsigned idx=0; idx < length(); idx++) {
            if (!JS_IsString((item = at(idx)))) continue;
            if (!(cstr = JS_ToCString(_ctx, item))) continue;
            result.push_back(cstr);
            JS_FreeCString(_ctx, cstr);
        }
    }
};

class JQArrayBufferView: public JQValue {
public:
    JQArrayBufferView(JSContext *ctx, JSValueConst value);
    ~JQArrayBufferView();
    size_t ByteLength() { return _length; }
    size_t ByteOffset() { return _offset; }
    size_t BytesPerElement() { return _bytes_per_element; }
    uint8_t* Data() { return _data + _offset; }
    uint8_t* BufferData() { return _data; }

private:
    size_t _length;
    size_t _offset;
    size_t _bytes_per_element;
    JSValue _array_buffer;

    uint8_t *_data;
    size_t _data_size;
};

template<typename T>
class JQScopeValue: public T {
public:
    JQScopeValue(JSContext *ctx, JSValue value)
            :T(ctx, value) { T::_setScope(true); }
};

using JQNumberScope = JQScopeValue<JQNumber>;
using JQStringScope = JQScopeValue<JQString>;
using JQArrayScope = JQScopeValue<JQArray>;
using JQArrayBufferViewScope = JQScopeValue<JQArrayBufferView>;
using JQObjectScope = JQScopeValue<JQObject>;
using JQBoolScope = JQScopeValue<JQBool>;

}  // namespace jqutil_v2