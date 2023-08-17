/**
 * @file json.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_JSON_H
#define CX_COMMON_JSON_H

#include <memory>
#include <cJSON.h>
#include <cx/hardware/board/board_params.h>

// struct Json;

// struct cJSON;
namespace cx {

using Json = cJSON *;

Json JsonCreate(const char *str);
void JsonDelete(Json obj);

Json JsonGetObjByPath(Json object, const char *path);
size_t JsonArraySize(Json object, const char *path);

template<typename T>
int JsonGetObjVal(Json object, const char *name, T &val)
{
    Json obj = JsonGetObjByPath(object, name);
    if (cJSON_IsNumber(obj)) {
        val = static_cast<T>(obj->valuedouble);
        return 0;
    }

    return -1;
}

template<> int JsonGetObjVal<bool>(Json object, const char *path, bool &val);
template<> int JsonGetObjVal<double>(Json object, const char *path, double &val);
template<> int JsonGetObjVal<int>(Json object, const char *path, int &val);
template<> int JsonGetObjVal<std::string>(Json object, const char *path, std::string &val);
// template<> int JsonGetObjVal<board::SensorConfig>(Json object, const char *path, board::SensorConfig &val);
template<> int JsonGetObjVal<board::SensorConfig::Tag>(Json object, const char *path, board::SensorConfig::Tag &val);

}
#endif /* CX_COMMON_JSON_H */

