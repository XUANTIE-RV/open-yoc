#include "JSSystemKVImpl.h"
#include "ulog/ulog.h"

#define TAG "JS_SystemKv"
namespace aiot
{
/**
 * 设置一个存储项
 * @param key
 * @param value
 */
void JSSystemKVImpl::setItem(std::string key, std::string value)
{
    // TODO  setItem
    if(!aos_kv_setstring(key.c_str(), value.c_str())){
        LOGI(TAG, "KV save succes");
    }else{
        LOGE(TAG, "KV save fail");
    }
}

/**
 * 获取指定 key 的存储项的值
 * @param key
 * @param defaultValue 如果 key 不存在，则返回 defaultValue。
 * @return
 */
std::string JSSystemKVImpl::getItem(std::string key, std::string defaultValue)
{
    // TODO  getItem
    size_t buf_size = 512;
    char value_buf[buf_size];
    if (aos_kv_getstring(key.c_str(), value_buf, buf_size) > 0){
        LOGI(TAG, "KV get success!");
        std::cout << value_buf << std::endl;
        return value_buf;
    }else{
        LOGE(TAG, "KV get fail!");
        if (defaultValue.empty()) {
            return "undefined";
        }
    return defaultValue;
    }
}

/**
 *  删除名称为 key 的存储项
 * @param key
 */
void JSSystemKVImpl::removeItem(std::string key)
{
    // TODO  removeItem
    if(!aos_kv_del(key.c_str())){
        LOGE(TAG, "KV del succes");
    }else{
        LOGE(TAG, "KV del fail");
    }
}

/**
 * 清除所有存储项
 */
void JSSystemKVImpl::clear()
{
    // TODO  clear
    if(!aos_kv_reset()){
        LOGE(TAG, "KV reset succes");
    }else{
        LOGE(TAG, "KV reset fail");
    }
}
}  // namespace aiot
