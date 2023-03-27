/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */
#ifndef ___JQUICK_BASE_REFBASE_H___
#define ___JQUICK_BASE_REFBASE_H___

#include <stdio.h>
#include <stdlib.h>
//#include "utils/log.h"
#include "utils/atomic.h"
#include "utils/StrongPtr.h"

//#define JQUICK_DEBUG_REF 1

namespace JQuick
{
class REF_BASE;
class weakref_impl;

class weakref_impl
{
public:
    inline weakref_impl(REF_BASE* ref) :
            m_strongrefCount(0), m_weakrefCount(0), m_ref(ref)
    {
    }

    inline virtual ~weakref_impl()
    {
    }

    inline REF_BASE* get()
    {
        if (m_ref) {
            return m_ref;
        } else {
            return NULL;
        }
    }

    inline weakref_impl* ref_weak()
    {
        jquick_atomic_inc(&m_weakrefCount);
#ifdef JQUICK_DEBUG_REF
        int count = m_weakrefCount;
        LOGI("ref_weak refCount=%d this=%p ref=%p", count, this, m_ref);
#endif
        return this;
    }

    inline void deref_weak()
    {
#ifdef JQUICK_DEBUG_REF
        int count = m_weakrefCount - 1;
        LOGI("deref_weak refCount=%d this=%p ref=%p", count, this, m_ref);
#endif
        const int32_t c = jquick_atomic_dec(&m_weakrefCount);
        if (c == 1) {
            delete this;
        }
    }

    inline bool attempt_refStrong()
    {
        ref_weak();
        const int32_t c = jquick_atomic_inc(&m_strongrefCount);
        bool r = c >= 1;
        if (!r) {
            deref_weak();
            jquick_atomic_dec(&m_strongrefCount);
        }
        return r;
    }

private:
    friend class REF_BASE;
    volatile int32_t m_strongrefCount;
    int32_t m_weakrefCount;
    REF_BASE* m_ref;
};

class REF_BASE
{
public:
    virtual ~REF_BASE()
    {
    }

    inline void REF()
    {
        jquick_atomic_inc(&m_weakref->m_strongrefCount);
        m_weakref->ref_weak();
#ifdef JQUICK_DEBUG_REF
        int count = getStrongCount();
        LOGI("ref refCount=%d this=%p", count, this);
#endif
    }
    inline void UNREF()
    {
        weakref_impl* weakref = m_weakref;
#ifdef JQUICK_DEBUG_REF
        int count = getStrongCount() - 1;
        LOGI("deref refCount=%d this=%p weak=%p", count, this, weakref);
#endif
        const int32_t c = jquick_atomic_dec(&weakref->m_strongrefCount);
        if (c == 1) {
            weakref->m_ref = NULL;
        }
        weakref->deref_weak();
        if (c == 1) {
            delete this;
        }
    }

    inline int32_t getStrongCount() const
    {
        return m_weakref->m_strongrefCount;
    }

    inline weakref_impl* get_weak_impl()
    {
        return m_weakref;
    }

protected:
    REF_BASE() :
            m_weakref(new weakref_impl(this)){};

private:
    friend class weakref_impl;

    weakref_impl* m_weakref;
};

template < typename T >
class sp;

#define COMPARE(_op_)                                 \
    inline bool operator _op_(const wp< T >& o) const \
    {                                                 \
        return m_ptr _op_ o.m_ptr;                    \
    }                                                 \
    inline bool operator _op_(const T* o) const       \
    {                                                 \
        return get() _op_ o;                          \
    }                                                 \
    template < typename U >                           \
    inline bool operator _op_(const wp< U >& o) const \
    {                                                 \
        return m_ptr _op_ o.m_ptr;                    \
    }                                                 \
    template < typename U >                           \
    inline bool operator _op_(const U* o) const       \
    {                                                 \
        return get() _op_ o;                          \
    }                                                 \
    inline bool operator _op_(const sp< T >& o) const \
    {                                                 \
        return get() _op_ o.m_ptr;                    \
    }                                                 \
    template < typename U >                           \
    inline bool operator _op_(const sp< U >& o) const \
    {                                                 \
        return get() _op_ o.m_ptr;                    \
    }

// ---------------------------------------------------------------------------

template < typename T >
class wp
{
public:
    inline wp() :
            m_ptr(NULL) {}

    wp(T* other);
    wp(const wp< T >& other);
    wp(const sp< T >& other);
    template < typename U >
    wp(U* other);
    template < typename U >
    wp(const wp< U >& other);
    template < typename U >
    wp(const sp< U >& other);

    ~wp();

    // Assignment

    wp& operator=(T* other);
    wp& operator=(const wp< T >& other);
    wp& operator=(const sp< T >& other);

    template < typename U >
    wp& operator=(const wp< U >& other);
    template < typename U >
    wp& operator=(U* other);

    template < typename U >
    wp& operator=(const sp< U >& other);

    // Reset

    void clear();

    // Accessors

    inline T& operator*() const
    {
        return *get();
    }
    inline T* operator->() const
    {
        return get();
    }
    inline T* get() const
    {
        return m_ptr ? (T*)m_ptr->get() : NULL;
    }
    inline sp< T > promote() const
    {
        if (m_ptr && m_ptr->attempt_refStrong()) {
            sp< T > result;
            result.set_pointer((T*)m_ptr->get());
            return result;
        }
        return NULL;
    }

    // Operators

    COMPARE(==)
    COMPARE(!=)
    COMPARE(>)
    COMPARE(<)
    COMPARE(<=)
    COMPARE(>=)

private:
    template < typename Y >
    friend class sp;
    template < typename Y >
    friend class wp;
    weakref_impl* m_ptr;
};

#undef COMPARE

// ---------------------------------------------------------------------------
// No user serviceable parts below here.

template < typename T >
wp< T >::wp(T* other)
{
    if (other) {
        m_ptr = other->get_weak_impl();
        m_ptr->ref_weak();
    } else {
        m_ptr = NULL;
    }
}

template < typename T >
wp< T >::wp(const wp< T >& other) :
        m_ptr(other.m_ptr)
{
    if (m_ptr) {
        m_ptr->ref_weak();
    }
}

template < typename T >
wp< T >::wp(const sp< T >& other)
{
    T* otherPtr(other.get());
    m_ptr = otherPtr ? otherPtr->get_weak_impl() : NULL;
    if (m_ptr) {
        m_ptr->ref_weak();
    }
}

template < typename T >
template < typename U >
wp< T >::wp(U* other)
{
    if (other) {
        m_ptr = ((T*)other)->get_weak_impl();
        m_ptr->ref_weak();
    } else {
        m_ptr = NULL;
    }
}

template < typename T >
template < typename U >
wp< T >::wp(const wp< U >& other) :
        m_ptr(other.m_ptr)
{
    if (m_ptr) {
        m_ptr->ref_weak();
    }
}

template < typename T >
template < typename U >
wp< T >::wp(const sp< U >& other)
{
    T* otherPtr(other.get());
    m_ptr = otherPtr ? otherPtr->get_weak_impl() : NULL;
    if (m_ptr) {
        m_ptr->ref_weak();
    }
}

template < typename T >
wp< T >::~wp()
{
    if (m_ptr) {
        m_ptr->deref_weak();
    }
}

template < typename T >
wp< T >& wp< T >::operator=(const wp< T >& other)
{
    T* otherPtr(other.get());
    weakref_impl* old = m_ptr;
    m_ptr = otherPtr ? otherPtr->get_weak_impl() : NULL;
    if (m_ptr) {
        m_ptr->ref_weak();
    }
    if (old) {
        old->deref_weak();
    }
    return *this;
}

template < typename T >
wp< T >& wp< T >::operator=(T* other)
{
    weakref_impl* old = m_ptr;
    m_ptr = other ? other->get_weak_impl() : NULL;
    if (m_ptr) {
        m_ptr->ref_weak();
    }
    if (old) {
        old->deref_weak();
    }
    return *this;
}

template < typename T >
template < typename U >
wp< T >& wp< T >::operator=(const wp< U >& other)
{
    T* otherPtr(other.get());
    weakref_impl* old = m_ptr;
    m_ptr = otherPtr ? otherPtr->get_weak_impl() : NULL;
    if (m_ptr) {
        m_ptr->ref_weak();
    }
    if (old) {
        old->deref_weak();
    }
    return *this;
}

template < typename T >
template < typename U >
wp< T >& wp< T >::operator=(U* other)
{
    weakref_impl* old = m_ptr;
    m_ptr = other ? other->get_weak_impl() : NULL;
    if (m_ptr) {
        m_ptr->ref_weak();
    }
    if (old) {
        old->deref_weak();
    }
    return *this;
}

template < typename T >
wp< T >& wp< T >::operator=(const sp< T >& other)
{
    T* otherPtr(other.get());
    weakref_impl* old = m_ptr;
    m_ptr = otherPtr ? otherPtr->get_weak_impl() : NULL;
    if (m_ptr) {
        m_ptr->ref_weak();
    }
    if (old) {
        old->deref_weak();
    }
    return *this;
}
template < typename T >
template < typename U >
wp< T >& wp< T >::operator=(const sp< U >& other)
{
    T* otherPtr(other.get());
    weakref_impl* old = m_ptr;
    m_ptr = otherPtr ? otherPtr->get_weak_impl() : NULL;
    if (m_ptr) {
        m_ptr->ref_weak();
    }
    if (old) {
        old->deref_weak();
    }
    return *this;
}

template < typename T >
void wp< T >::clear()
{
    T* ptr = get();
    if (ptr) {
        ptr->deref_weak();
    }
}

class AutoRefCount
{
public:
    AutoRefCount(REF_BASE* ref)
    {
        ref->REF();
        _ref = ref;
    }
    ~AutoRefCount()
    {
        _ref->UNREF();
    }

private:
    AutoRefCount(const AutoRefCount& o);
    AutoRefCount& operator=(const AutoRefCount&);

private:
    REF_BASE* _ref;
};
class AutoUnref
{
public:
    AutoUnref(REF_BASE* ref)
    {
        _ref = ref;
    }
    ~AutoUnref()
    {
        if (_ref) {
            _ref->UNREF();
        }
    }

private:
    AutoUnref(const AutoRefCount& o);
    AutoUnref& operator=(const AutoUnref&);

private:
    REF_BASE* _ref;
};

#define SAFE_ASSIGN(REF_L, REF_R) \
    if ((REF_L) != (REF_R)) {     \
        SAFE_UNREF(REF_L)         \
        SAFE_REF(REF_R)           \
        REF_L = REF_R;            \
    }

#define SAFE_REF(r) \
    if (r) {        \
        r->REF();   \
    }

#define SAFE_UNREF(r) \
    if (r) {          \
        r->UNREF();   \
    }

}  // namespace JQuick

#endif
