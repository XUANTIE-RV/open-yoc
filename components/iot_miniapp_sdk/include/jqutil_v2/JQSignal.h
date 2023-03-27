#pragma once

// NOTE: source from https://schneegans.github.io/tutorials/2015/09/20/signal-slot
//  mutex reference from https://github.com/palacaze/sigslot/blob/master/include/sigslot/signal.hpp
//  keep it simple
#include <atomic>
#include <mutex>
#include <functional>
#include <map>
#include <assert.h>
#include "JQuickContext.h"

namespace jqutil_v2 {

class JQSignal_C11FuncTask : public JQuick::Task
{
public:
    JQSignal_C11FuncTask(std::function<void()> func)
            : _func(func)
    {
    }
    virtual ~JQSignal_C11FuncTask() {}
    virtual void run()
    {
        if (_func) _func();
    }

private:
    std::function<void()> _func;
};

// A JQSignal object may call multiple slots with the
// same signature. You can connect functions to the JQSignal
// which will be called when the emit() method on the
// JQSignal object is invoked. Any argument passed to emit()
// will be passed to the given functions.

typedef enum {
    JQ_SIGNAL_CONN_TYPE_AUTO,
    JQ_SIGNAL_CONN_TYPE_ASYNC,
    JQ_SIGNAL_CONN_TYPE_SYNC,
} JQSignalConnType;

template <typename... Args>
class JQSignal {
public:
    using FuncType = std::function<void(Args...)>;

private:
    using lock_type = std::unique_lock<std::mutex>;

    typedef enum {
        TARGET_TYPE_GENERIC,
        TARGET_TYPE_MEMBER,
    } TargetType;

    struct Connection {
        int id;
        FuncType func;
        TargetType targetType;
        JQSignalConnType connType;

        JQuick::wp<JQuick::REF_BASE> instWeakPtr;
    };

public:
    // JQSignal()  = default;
    JQSignal() { _initHandler( new JQuick::Handler(JQuick::Looper::myLooper()) ); }
    // ~JQSignal() = default;
    ~JQSignal() { _handler->removeTasksAndMessages(); }

    // Copy constructor and assignment create a new JQSignal.
    JQSignal(JQSignal const& /*unused*/) {}

    JQSignal& operator=(JQSignal const& other) {
        if (this != &other) {
            disconnect_all();
        }
        return *this;
    }

    // Move constructor and assignment operator work as expected.
//    JQSignal(JQSignal&& other) noexcept:
//            _slots(std::move(other._slots)),
//            _current_id(other._current_id) {}
    JQSignal(JQSignal&& other) noexcept
    {
        lock_type lock(other.m_mutex);
        _slots = std::move(other._slots);
        _current_id = other._current_id;
    }

    JQSignal& operator=(JQSignal&& other) noexcept {
        if (this != &other) {
            lock_type lock1(m_mutex, std::defer_lock);
            lock_type lock2(other.m_mutex, std::defer_lock);
            std::lock(lock1, lock2);

            _slots     = std::move(other._slots);
            _current_id = other._current_id;
        }

        return *this;
    }


    // Connects a std::function to the JQSignal. The returned
    // value can be used to disconnect the function again.
    inline int connect(FuncType const& slot, JQSignalConnType connType=JQ_SIGNAL_CONN_TYPE_AUTO) const {
        lock_type lock(m_mutex);
        return _connect(slot, connType, TARGET_TYPE_GENERIC).id;
    }

    // Convenience method to connect a member function of an
    // object to this JQSignal.
    template<typename T>
    inline int connect_member(T *inst, void (T::*func)(Args...),
            JQSignalConnType connType=JQ_SIGNAL_CONN_TYPE_AUTO) const {
        lock_type lock(m_mutex);
        assert(inst->handler() != nullptr);
        JQuick::wp<JQuick::REF_BASE> instWeakPtr = inst;
        Connection &conn = _connect([instWeakPtr, func](Args... args) {
            JQuick::sp<T> instPtr = instWeakPtr.promote();
            if (instPtr.get() != NULL) {
                (instPtr.get()->*func)(args...);
            }
        }, connType, TARGET_TYPE_MEMBER);
        conn.instWeakPtr = inst;
        return conn.id;
    }

    // Convenience method to connect a const member function
    // of an object to this JQSignal.
    template<typename T>
    inline int connect_member(T *inst, void (T::*func)(Args...) const,
            JQSignalConnType connType=JQ_SIGNAL_CONN_TYPE_AUTO) const {
        lock_type lock(m_mutex);
        assert(inst->handler() != nullptr);
        JQuick::wp<JQuick::REF_BASE> instWeakPtr = inst;
        Connection &conn = _connect([instWeakPtr, func](Args... args) {
            JQuick::sp<T> instPtr = instWeakPtr.promote();
            if (instPtr.get() != NULL) {
                (instPtr.get()->*func)(args...);
            }
        }, connType, TARGET_TYPE_MEMBER);
        conn.instWeakPtr = inst;
        return conn.id;
    }

    // connect member function with magic
    template<typename T>
    inline int connect_member_magic(T *inst, void (T::*func)(intptr_t magic, Args...),
            JQSignalConnType connType=JQ_SIGNAL_CONN_TYPE_AUTO, intptr_t magic=0) const {
        lock_type lock(m_mutex);
        assert(inst->jsHandler() != nullptr);
        JQuick::wp<T> instWeakPtr = inst;
        Connection &conn = _connect([instWeakPtr, func, magic](Args... args) {
            JQuick::sp<T> instPtr = instWeakPtr.promote();
            if (instPtr.get() != NULL) {
                (instPtr.get()->*func)(magic, args...);
            }
        }, connType, TARGET_TYPE_MEMBER);
        conn.instWeakPtr = inst;
        return conn.id;
    }

    template<typename T>
    inline int connect_member_magic(T *inst, void (T::*func)(intptr_t magic, Args...) const,
            JQSignalConnType connType=JQ_SIGNAL_CONN_TYPE_AUTO, intptr_t magic=0) const {
        lock_type lock(m_mutex);
        assert(inst->handler() != nullptr);
        JQuick::wp<JQuick::REF_BASE> instWeakPtr = inst;
        Connection &conn = _connect([instWeakPtr, func, magic](Args... args) {
            JQuick::sp<T> instPtr = instWeakPtr.promote();
            if (instPtr.get() != NULL) {
                (instPtr.get()->*func)(magic, args...);
            }
        }, connType, TARGET_TYPE_MEMBER);
        conn.instWeakPtr = inst;
        return conn.id;
    }

    // disconnects a previously connected function.
    inline void disconnect(int id) const {
        lock_type lock(m_mutex);
        _slots.erase(id);
    }

    // disconnects all previously connected functions.
    inline void disconnect_all() const {
        lock_type lock(m_mutex);
        _slots.clear();
    }

    // calls all connected functions.
    inline void emit(Args... p) const {
        std::map<int, Connection> slots = slots_reference();
        for (auto const& it : slots) {
            _call(it.second, p...);
        }
    }

    // Calls all connected functions except for one.
    inline void emit_for_all_but_one(int excludedConnectionID, Args... p) const {
        std::map<int, FuncType> slots = slots_reference();
        for (auto const& it : slots) {
            if (it.first != excludedConnectionID) {
                _call(it.second, p...);
            }
        }
    }

    // calls only one connected function.
    inline void emit_for(int connectionID, Args... p) const {
        Connection conn;
        conn.id = 0;
        {
            lock_type lock(m_mutex);
            auto const& it = _slots.find(connectionID);
            if (it != _slots.end()) conn = it->second;
        }
        if (conn.id != 0) {
            _call(conn, p...);
        }
    }

    // used to get a reference to the slots for reading
    inline std::map<int, Connection> slots_reference() const {
        lock_type lock(m_mutex);
        return _slots;
    }

    // thread relative
    inline bool inSameThread() const
    {
        return jquick_thread_get_current() == _tid;
    }

    inline JQuick_Thread threadId() const
    {
        return _tid;
    }

    JQuick::sp<JQuick::Handler> handler()
    {
        return _handler;
    }

    void setHandler(JQuick::sp<JQuick::Handler> handler)
    {
        _initHandler(handler);
    }
    void setLooper(JQuick::sp< JQuick::Looper > looper)
    {
        _initHandler(new JQuick::Handler(looper));
    }

private:
    void _initHandler(const JQuick::sp<JQuick::Handler> handler)
    {
        _handler = handler;
        _tid = handler->getLooper()->getThreadId();
    }

    inline Connection& _connect(FuncType const& slot, JQSignalConnType connType, TargetType targetType) const {
        return _slots.emplace(std::make_pair(_current_id, Connection({
            .id=++_current_id,
            .func = slot,
            .targetType=targetType,
            .connType=connType,
            .instWeakPtr=nullptr
        }))).first->second;
    }

    inline void _call(const Connection &conn, Args... p) const
    {
        if (conn.connType == JQ_SIGNAL_CONN_TYPE_AUTO) {
            // according to signal
            if (jquick_thread_get_current() == _tid) {
                // call it directly
                conn.func(p...);
            } else {
                _handler->run(new JQSignal_C11FuncTask([=]() {
                  conn.func(p...);
                }));
            }
        } else if (conn.connType == JQ_SIGNAL_CONN_TYPE_ASYNC) {
            // according to signal
            _handler->run(new JQSignal_C11FuncTask([=]() {
              conn.func(p...);
            }));
        } else { // (conn.connType == JQ_SIGNAL_CONN_TYPE_SYNC)
            // force to execute
            conn.func(p...);
        }
    }

private:
    mutable std::mutex                  m_mutex;
    mutable std::map<int, Connection >  _slots;
    mutable int                         _current_id{0};

    JQuick::sp<JQuick::Handler>         _handler;
    JQuick_Thread                       _tid;
};

}  // namespace jqutil_v2