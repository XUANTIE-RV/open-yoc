/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_PROPERTY_H
#define TM_PROPERTY_H

#include <assert.h>
#include <string.h>
#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <map>
#include <vector>

using namespace std;
#define TM_PROPERTY_STRING_MAX_LEN 128

class TMProperty
{
public:
    int32_t        Id;

    enum class PropertyType
    {
        UNKNOWN  = 0,
        INT32,
        UINT32,
        BOOLEAN,
        STRING,
    } Type;

    union PropertyValue
    {
        bool     Bool;
        int32_t  Int;
        uint32_t Uint;
        char     String[TM_PROPERTY_STRING_MAX_LEN];
    } Value;

    string Name;

    TMProperty();
    TMProperty(int id, int32_t     value, string name = "");
    TMProperty(int id, uint32_t    value, string name = "");
    TMProperty(int id, bool        value, string name = "");
    TMProperty(int id, string      value, string name = "");
    TMProperty(int id, const char *value, string name = "");
    TMProperty(int id, char       *value, string name = "");
    ~TMProperty();

    void Assign(TMProperty *property);   // TODO: Modify to be operator =
    void Assign(int32_t  value);
    void Assign(uint32_t value);
    void Assign(bool     value);
    void Assign(char    *value);
    void Dump(int alignNameLen = -1);
};

typedef map<int, TMProperty> TMPropertyMap_t;

class TMPropertyList
{
public:
    TMPropertyMap_t mProperties;

    int Add(TMProperty property);
    int Add(TMProperty *property);

    template<typename T> TMProperty *Get(T propId);
    template<typename T> int Get(T propId, int *value);
    template<typename T> int Get(T propId, uint32_t *value);
    template<typename T> int Get(T propId, bool *value);
    template<typename T> int Get(T propId, char *value);

    template<typename T> int32_t     GetInt(T propId);
    template<typename T> uint32_t    GetUint(T propId);
    template<typename T> bool        GetBool(T propId);
    template<typename T> char       *GetString(T propId);

    template<typename T1, typename T2> int Assign(T1 propId, T2 value);

    void Reset();

    int Count();
    int FindDiff(vector<int> &diffIDArray, TMPropertyList *searchList);
    void Dump();

private:
    bool Find(int Id);
};

template<typename T>
TMProperty *TMPropertyList::Get(T propId)
{
    int innerPropID = static_cast<int>(propId);
    auto iter = mProperties.find(innerPropID);
    if (iter == mProperties.end())
        return NULL;
    else
        return &(iter->second);
}

template<typename T>
int TMPropertyList::Get(T propId, int *value)
{
    int innerPropID = static_cast<int>(propId);
    auto iter = mProperties.find(innerPropID);
    if (iter == mProperties.end())
        return TMResult::TM_PROPERTY_NOT_FOUND;

    TMProperty *prop = &(iter->second);
    *value = prop->Value.Int;

    return 0;
}

template<typename T>
int TMPropertyList::Get(T propId, uint32_t *value)
{
    int innerPropID = static_cast<int>(propId);
    auto iter = mProperties.find(innerPropID);
    if (iter == mProperties.end())
        return TMResult::TM_PROPERTY_NOT_FOUND;

    TMProperty *prop = &(iter->second);
    *value = prop->Value.Uint;
    return 0;
}

template<typename T>
int TMPropertyList::Get(T propId, bool *value)
{
    int innerPropID = static_cast<int>(propId);
    auto iter = mProperties.find(innerPropID);
    if (iter == mProperties.end())
        return TMResult::TM_PROPERTY_NOT_FOUND;

    TMProperty *prop = &(iter->second);
    *value = prop->Value.Bool;
    return 0;
}

template<typename T>
int TMPropertyList::Get(T propId, char *value)
{
    int innerPropID = static_cast<int>(propId);
    auto iter = mProperties.find(innerPropID);
    if (iter == mProperties.end())
        return TMResult::TM_PROPERTY_NOT_FOUND;

    TMProperty *prop = &(iter->second);
    strncpy(value, prop->Value.String, sizeof(prop->Value.String));
    return 0;
}

template<typename T>
int32_t TMPropertyList::GetInt(T propId)
{
    int innerPropID = static_cast<int>(propId);
    TMProperty *property = Get(innerPropID);
    assert(property != NULL);

    return property->Value.Int;
}

template<typename T>
uint32_t TMPropertyList::GetUint(T propId)
{
    int innerPropID = static_cast<int>(propId);
    TMProperty *property = Get(innerPropID);
    assert(property != NULL);

    return property->Value.Uint;
}

template<typename T>
bool TMPropertyList::GetBool(T propId)
{
    int innerPropID = static_cast<int>(propId);
    TMProperty *property = Get(innerPropID);
    assert(property != NULL);

    return property->Value.Bool;
}

template<typename T>
char *TMPropertyList::GetString(T propId)
{
    int innerPropID = static_cast<int>(propId);
    TMProperty *property = Get(innerPropID);
    assert(property != NULL);

    return property->Value.String;
}

template<typename T1, typename T2>
int TMPropertyList::Assign(T1 propId, T2 value)
{
    int innerPropID = static_cast<int>(propId);
    TMProperty *property = Get(innerPropID);
    if (property == NULL)
    {
        property = &(this->mProperties[innerPropID]);  // As create a new element
        property->Id = innerPropID;
    }

    if (typeid(value) == typeid(const char*))
    {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
        property->Assign((char *)value);
    #pragma GCC diagnostic pop
    }
    else
        property->Assign(value);

    return 0;
}

#endif  /* TM_PROPERTY_H */
