/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */
#ifndef ___JQUICK_LRUCACHE_H___
#define ___JQUICK_LRUCACHE_H___

#include <string>
#include <unordered_map>
#include <list>
#include <functional>

#include "utils/Mutex.h"

namespace JQuick
{
template < typename CKey, typename CValue >
class LruCacheDelegator {
public:
    virtual ~LruCacheDelegator() { };
    virtual int32_t sizeOfEntry(const CKey& key, const CValue& value) { return 1; };
    virtual uint32_t hash(const CKey& key) = 0;
    virtual void onEntryRemoved(const CKey& key, const CValue& value) {};
};
template < typename CKey, typename CValue >
class LruCache
{
public:
    LruCache(int32_t maxSize, LruCacheDelegator< CKey, CValue >* delegator = NULL);
    ~LruCache();
    bool putCache(const CKey& key, const CValue& v);
    bool getCache(const CKey& key, CValue& v);

    void resize(int32_t maxSize);
    void erase();
    void trim(float rate);
    bool available() const;
    int32_t getCacheSize() const;
    int32_t getMaxCacheSize() const;
private:
    void removeEldest();

private:
    int32_t _maxSize;
    int32_t _cacheSize;
    LruCacheDelegator< CKey, CValue >* _delegator;
    std::unordered_map< CKey, typename std::list< std::pair< CKey, CValue > >::iterator, std::function<size_t( const CKey& p )> > _cacheMap;
    std::list< std::pair< CKey, CValue > > _lruList;

    JQuick::Mutex _mutex;
};

template < typename CKey, typename CValue >
LruCache< CKey, CValue >::LruCache(int32_t maxSize, LruCacheDelegator< CKey, CValue >* delegator) :
        _maxSize(maxSize),
        _cacheSize(0),
        _delegator(delegator),
        _cacheMap(10, [delegator](const CKey& key) {
            // hash计算
            if (delegator != NULL) {
                return std::size_t(delegator->hash(key));
            }
            return std::size_t(0);
        })
{
    if (_maxSize < 0) {
        _maxSize = 0;
    }
}
template < typename CKey, typename CValue >
LruCache< CKey, CValue >::~LruCache()
{
    erase();
}
template < typename CKey, typename CValue >
bool LruCache< CKey, CValue >::putCache(const CKey& key, const CValue& v)
{
    JQuick::Mutex::Autolock l(_mutex);
    int32_t size;
    if (_delegator != NULL) {
        size = _delegator->sizeOfEntry(key, v);
    } else {
        size = 1;
    }
    if (size <= 0 || size > _maxSize) {
        return false;
    }
    auto iter = _cacheMap.find(key);
    if (iter != _cacheMap.end()) {
        return false;
    }
    while (_cacheSize + size > _maxSize) {
        removeEldest();
    }
    _lruList.push_front(std::pair< CKey, CValue >(key, v));
    _cacheMap[key] = _lruList.begin();
    _cacheSize += size;
    return true;
}

template < typename CKey, typename CValue >
bool LruCache< CKey, CValue >::getCache(const CKey& key, CValue& v)
{
    JQuick::Mutex::Autolock l(_mutex);
    auto iter = _cacheMap.find(key);
    if (iter != _cacheMap.end()) {
        const CValue& result = iter->second->second;
        _lruList.splice(_lruList.begin(), _lruList, iter->second);
        iter->second = _lruList.begin();
        v = result;
        return true;
    }
    return false;
}
template < typename CKey, typename CValue >
void LruCache< CKey, CValue >::resize(int32_t maxSize)
{
    if (maxSize < 0) maxSize = 0;
    if (_maxSize != maxSize) {
        _maxSize = maxSize;
        JQuick::Mutex::Autolock l(_mutex);
        while (_cacheSize > _maxSize) {
            removeEldest();
        }
    }
}
template < typename CKey, typename CValue >
void LruCache< CKey, CValue >::erase()
{
    JQuick::Mutex::Autolock l(_mutex);
    _cacheMap.clear();
    for (typename std::list< std::pair< CKey, CValue > >::iterator iter = _lruList.begin(); iter != _lruList.end(); iter++) {
        if (_delegator != NULL) {
            _delegator->onEntryRemoved(iter->first, iter->second);
        }
    }
    _lruList.clear();
    _cacheSize = 0;
}
template < typename CKey, typename CValue >
void LruCache< CKey, CValue >::trim(float rate)
{
    int32_t newSize = int32_t(_maxSize * rate);
    if (newSize <= 0) {
        erase();
        return;
    }
    if (newSize >= _maxSize) {
        return;
    }
    JQuick::Mutex::Autolock l(_mutex);
    while (_cacheSize > newSize) {
        removeEldest();
    }
}
template < typename CKey, typename CValue >
bool LruCache< CKey, CValue >::available() const
{
    return _maxSize > 0;
}
template < typename CKey, typename CValue >
int32_t LruCache< CKey, CValue >::getCacheSize() const
{
    return _cacheSize;
}
template < typename CKey, typename CValue >
int32_t LruCache< CKey, CValue >::getMaxCacheSize() const
{
    return _maxSize;
}
template < typename CKey, typename CValue >
void LruCache< CKey, CValue >::removeEldest()
{
    if (!_lruList.empty()) {
        const std::pair< CKey, CValue >& item = _lruList.back();
        const CValue& removed = item.second;
        if (_delegator != NULL) {
            _cacheSize -= _delegator->sizeOfEntry(item.first, item.second);
            _delegator->onEntryRemoved(item.first, item.second);
        } else {
            _cacheSize -= 1;
        }
        _cacheMap.erase(item.first);
        _lruList.pop_back();
    }
}
}  // namespace JQuick

#endif  //___JQUICK_LRUCACHE_H___
