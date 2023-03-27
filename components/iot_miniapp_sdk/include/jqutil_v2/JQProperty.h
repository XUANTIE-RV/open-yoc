#pragma once

#include "jqutil_v2/JQSignal.h"

#include <iostream>

namespace jqutil_v2 {

// A JQProperty is a encpsulates a value and may inform
// you on any changes applied to this value.

template <typename T>
class JQProperty {

public:
    typedef T value_type;

    // Properties for built-in types are automatically
    // initialized to 0. See template spezialisations
    // at the bottom of this file
    JQProperty()
            : connection_(nullptr)
            , connection_id_(-1) {}

    JQProperty(T const& val)
            : value_(val)
            , connection_(nullptr)
            , connection_id_(-1) {}

    JQProperty(T&& val)
            : value_(std::move(val))
            , connection_(nullptr)
            , connection_id_(-1) {}

    JQProperty(JQProperty<T> const& to_copy)
            : value_(to_copy.value_)
            , connection_(nullptr)
            , connection_id_(-1) {}

    JQProperty(JQProperty<T>&& to_copy)
            : value_(std::move(to_copy.value_))
            , connection_(nullptr)
            , connection_id_(-1) {}

    // returns a JQSignal which is fired when the internal value
    // will be changed. The old value is passed as parameter.
    virtual JQSignal<T> const& before_change() const { return before_change_; }

    // returns a JQSignal which is fired when the internal value
    // has been changed. The new value is passed as parameter.
    virtual JQSignal<T> const& on_change() const { return on_change_; }

    // sets the JQProperty to a new value. before_change() and
    // on_change() will be emitted.
    virtual void set(T const& value) {
        if (value != value_) {
            before_change_.emit(value_);
            value_ = value;
            on_change_.emit(value_);
        }
    }

    // sets the JQProperty to a new value. before_change() and
    // on_change() will not be emitted
    void set_with_no_emit(T const& value) {
        value_ = value;
    }

    // emits before_change() and on_change() even if the value
    // did not change
    void touch() {
        before_change_.emit(value_);
        on_change_.emit(value_);
    }

    // returns the internal value
    virtual T const& get() const { return value_; }

    // connects two Properties to each other. If the source's
    // value is changed, this' value will be changed as well
    virtual void connect_from(JQProperty<T> const& source) {
        disconnect();
        connection_ = &source;
        connection_id_ = source.on_change().connect([this](T const& value){
          set(value);
          return true;
        });
        set(source.get());
    }

    // if this JQProperty is connected from another JQProperty,
    // it will e disconnected
    virtual void disconnect() {
        if (connection_) {
            connection_->on_change().disconnect(connection_id_);
            connection_id_ = -1;
            connection_ = nullptr;
        }
    }

    // if there are any Properties connected to this JQProperty,
    // they won't be notified of any further changes
    virtual void disconnect_auditors() {
        on_change_.disconnect_all();
        before_change_.disconnect_all();
    }

    // assigns the value of another JQProperty
    virtual JQProperty<T>& operator=(JQProperty<T> const& rhs) {
        set(rhs.value_);
        return *this;
    }

    // assigns a new value to this JQProperty
    virtual JQProperty<T>& operator=(T const& rhs) {
        set(rhs);
        return *this;
    }

    // compares the values of two Properties
    bool operator==(JQProperty<T> const& rhs) const {
        return JQProperty<T>::get() == rhs.get();
    }
    bool operator!=(JQProperty<T> const& rhs) const {
        return JQProperty<T>::get() != rhs.get();
    }

    // compares the values of the JQProperty to another value
    bool operator==(T const& rhs) const { return JQProperty<T>::get() == rhs; }
    bool operator!=(T const& rhs) const { return JQProperty<T>::get() != rhs; }

    // returns the value of this JQProperty
    T const& operator()() const { return JQProperty<T>::get(); }

private:
    JQSignal<T> on_change_;
    JQSignal<T> before_change_;

    JQProperty<T> const* connection_;
    int connection_id_;
    T value_;
};

// specialization for built-in default contructors
template<> inline JQProperty<double>::JQProperty()
        : connection_(nullptr), connection_id_(-1), value_(0.0) {}

template<> inline JQProperty<float>::JQProperty()
        : connection_(nullptr), connection_id_(-1), value_(0.f) {}

template<> inline JQProperty<short>::JQProperty()
        : connection_(nullptr), connection_id_(-1), value_(0) {}

template<> inline JQProperty<int>::JQProperty()
        : connection_(nullptr), connection_id_(-1), value_(0) {}

template<> inline JQProperty<char>::JQProperty()
        : connection_(nullptr), connection_id_(-1), value_(0) {}

template<> inline JQProperty<unsigned>::JQProperty()
        : connection_(nullptr), connection_id_(-1), value_(0) {}

template<> inline JQProperty<bool>::JQProperty()
        : connection_(nullptr), connection_id_(-1), value_(false) {}

// stream operators
template<typename T>
std::ostream& operator<<(std::ostream& out_stream, JQProperty<T> const& val) {
    out_stream << val.get();
    return out_stream;
}

template<typename T>
std::istream& operator>>(std::istream& in_stream, JQProperty<T>& val) {
    T tmp;
    in_stream >> tmp;
    val.set(tmp);
    return in_stream;
}

}  // namespace jqutil_v2