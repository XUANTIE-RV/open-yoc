#pragma once
#include <memory>  // std::shared_ptr
#include "utils/REF.h"

namespace jqutil_v2 {

template<typename T>
class keep_wrap_for_shared_ptr: public JQuick::REF_BASE {
public:
    keep_wrap_for_shared_ptr(std::shared_ptr<T> ptr) :_ptr(ptr) {}
    ~keep_wrap_for_shared_ptr() { /*_ptr will be auto released*/ }

protected:
    std::shared_ptr<T> _ptr;
};

template<typename T>
class keep_wrap_for_delete: public JQuick::REF_BASE {
public:
    keep_wrap_for_delete(T* ptr) :_ptr(ptr) {}
    ~keep_wrap_for_delete() { if (_ptr) delete _ptr; _ptr = nullptr; }

protected:
    T* _ptr;
};

template<typename T>
struct keep__is_null_pointer: public std::true_type {};

template<>
struct keep__is_null_pointer<std::nullptr_t>: public std::true_type {};

// support JQuick::REF_BASE/ std::shared_ptr/ C++ new ptr
template<typename T>
class KeepPtr {
public:
    // default case
    KeepPtr() :_obj(nullptr), _ref(nullptr) {}
    // for nullptr_t case
    template<typename T1, typename std::enable_if< keep__is_null_pointer<T1>::value >::type* = nullptr>
    KeepPtr(T1 sub) :_obj(nullptr), _ref(nullptr) {}

    // for JQuick::REF_BASE
    template<typename T1, typename std::enable_if< std::is_base_of<JQuick::REF_BASE, T1>::value >::type* = nullptr>
    KeepPtr(T1* sub)
    {
        _obj = static_cast<T*>(sub);
        _ref = static_cast<JQuick::REF_BASE*>(sub);
        if (_ref) _ref->REF();
    }

    // for JQuick::wp
    template<typename T1>
    KeepPtr(JQuick::wp<T1> sub)
    {
        JQuick::sp<T1> sp = sub.promote();
        _obj = sp.get();
        _ref = sp.get();
        if (_ref) _ref->REF();
    }

    // for JQuick::sp
    template<typename T1>
    KeepPtr(JQuick::sp<T1> sub)
    {
        _obj = sub.get();
        _ref = sub.get();
        if (_ref) _ref->REF();
    }

    // for std::shared_ptr
    template<typename T1>
    KeepPtr(std::shared_ptr<T1> sub)
    {
        _obj = sub.get();
        if (_obj) {
            _ref = new keep_wrap_for_shared_ptr<T1>(sub);
            _ref->REF();
        } else {
            _ref = nullptr;
        }
    }

    // for other(not JQuick::REF_BASE) C++ new ptr
    template<typename T1, typename std::enable_if< !std::is_base_of<JQuick::REF_BASE, T1>::value >::type* = nullptr>
    KeepPtr(T1* sub)
    {
        _obj = sub;
        if (_obj) {
            _ref = new keep_wrap_for_delete<T1>(sub);
            _ref->REF();
        } else {
            _ref = nullptr;
        }
    }

    KeepPtr& operator=(const KeepPtr& other)
    {
        if (_obj) _ref->UNREF();
        _obj = other._obj;
        _ref = other._ref;
        if (_obj) _ref->REF();
        return *this;
    }
    KeepPtr(const KeepPtr &other)
    {
        if (_obj) _ref->UNREF();
        _obj = other._obj;
        _ref = other._ref;
        if (_obj) _ref->REF();
    }
    ~KeepPtr()
    {
        if (_obj) {
            _ref->UNREF();
            _obj = nullptr;
            _ref = nullptr;
        }
    }

    operator bool() const { return get() != nullptr; }

    bool operator ==(const T* ptr) const
    { return get() == ptr; }

    // Accessors
    inline T& operator*() const
    { return *get(); }
    inline T* operator->() const
    { return get(); }
    inline T* get() const
    { return _obj; }

    inline void clear()
    {
        if (_obj) {
            _ref->UNREF();
            _ref = nullptr;
            _obj = nullptr;
        }
    }

private:
    T *_obj = nullptr;
    JQuick::REF_BASE *_ref = nullptr;
};

}  // namespace JQuick