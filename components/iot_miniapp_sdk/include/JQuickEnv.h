/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_JQUICKENV_H___
#define ___JQUICK_JQUICKENV_H___

#include <string>
#include <stdint.h>

namespace JQuick
{
/**
 * 获取所有$falcon.env内容，json格式
 *
 * Exp: {"platform":"Darwin","version":"1.2.0","apiVersion":1,"deviceModel":"HaaS UI","deviceWidth":800,"deviceHeight":480,"custom":{}}
 *
 * 除了$falcon.env.custom，其他字段为只读
 * JS端通过$falcon.env.custom.$set(k, v)来修改env，会自动持久化
 * C++通过JQuick::setCustomEnvXXX来修改env，会自动持久化
 *
 * 监听:
 *    JS: $falcon.on('system_env_custom_{key}', callback)
 *    C++: JQuick::addCustomEnvChangeListener(func)
 */
int8_t getEnv(std::string& v);

/**
 * 获取$falcon.env内容
 * 成功：return 0
 * 失败：return -1 （值不存在或类型不匹配）
 */
/* int32 */
int8_t getEnvInt(const std::string& k, int32_t& v);
/* bool */
int8_t getEnvBool(const std::string& k, bool& v);
/* double */
int8_t getEnvDouble(const std::string& k, double& v);
/* string */
int8_t getEnvString(const std::string& k, std::string& v);
/* json(json格式字符串) */
int8_t getEnvJson(const std::string& k, std::string& v);

/* Custom env begin */
/**
 * 获取$falcon.env.custom内容
 * 其他同上
 */
/* int32 */
int8_t getCustomEnvInt(const std::string& k, int32_t& v);
/* bool */
int8_t getCustomEnvBool(const std::string& k, bool &v);
/* double */
int8_t getCustomEnvDouble(const std::string& k, double& v);
/* string */
int8_t getCustomEnvString(const std::string& k, std::string& v);
/* json(json格式字符串) */
int8_t getCustomEnvJson(const std::string& k, std::string& v);

/**
 * 设置$falcon.env.custom
 */
/* int32 */
int8_t setCustomEnvInt(const std::string& k, int32_t v);
/* bool */
int8_t setCustomEnvBool(const std::string& k, bool v);
/* double */
int8_t setCustomEnvDouble(const std::string& k, double v);
/* string */
int8_t setCustomEnvString(const std::string& k, const std::string& v);
/* json(json格式字符串) */
int8_t setCustomEnvJson(const std::string& k, const std::string& v);

/**
 * 监听$falcon.env.custom变化
 */
typedef void (*CustomEnvChange)(const std::string& k);
int8_t addCustomEnvChangeListener(CustomEnvChange cb);
int8_t removeCustomEnvChangeListener(CustomEnvChange cb);
/* Custom env end */

}

#endif  // ___JQUICK_JQUICKENV_H___
