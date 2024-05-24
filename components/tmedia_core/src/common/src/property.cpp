/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <iostream>
#include <iomanip>
#include <string.h>

#include <tmedia_core/common/error.h>
#include <tmedia_core/common/property.h>

using namespace std;

TMProperty::TMProperty()
{
    Id = 0;
    Type = PropertyType::UNKNOWN;
    Value.Int = 0;
    Name = "";
}

TMProperty::~TMProperty()
{
}

TMProperty::TMProperty(int id, int32_t value, string name)
{
    Id = id;
    Type = PropertyType::INT32;
    Value.Int = value;
    Name = name;
}

TMProperty::TMProperty(int id, bool value, string name)
{
    Id = id;
    Type = PropertyType::BOOLEAN;
    Value.Bool = value;
    Name = name;
}

TMProperty::TMProperty(int id, uint32_t value, string name)
{
    Id = id;
    Type = PropertyType::UINT32;
    Value.Uint = value;
    Name = name;
}

TMProperty::TMProperty(int id, string value, string name)
{
    Id = id;
    Type = PropertyType::STRING;
    if (value.length() > sizeof(Value.String))
    {
        cout << "Error, input string length > Value.String storage space" << endl;
    }
    strncpy(Value.String, value.c_str(), sizeof(Value.String) - 1);
    Value.String[sizeof(Value.String) - 1] = '\0';
    Name = name;
}

TMProperty::TMProperty(int id, const char *value, string name)
{
    Id = id;
    Type = PropertyType::STRING;
    strncpy(Value.String, value, sizeof(Value.String) - 1);
    Value.String[sizeof(Value.String) - 1] = '\0';
    Name = name;
}

TMProperty::TMProperty(int id, char *value, string name)
{
    Id = id;
    Type = PropertyType::STRING;
    strncpy(Value.String, value, sizeof(Value.String) - 1);
    Value.String[sizeof(Value.String) - 1] = '\0';
    Name = name;
}

void TMProperty::Assign(TMProperty *property)
{
    Id = property->Id;
    Type = property->Type;
    Value = property->Value;
}

void TMProperty::Assign(int32_t value)
{
    Type = PropertyType::INT32;
    Value.Int = value;
}

void TMProperty::Assign(uint32_t value)
{
    Type = PropertyType::UINT32;
    Value.Uint = value;
}

void TMProperty::Assign(bool value)
{
    Type = PropertyType::BOOLEAN;
    Value.Bool = value;
}

void TMProperty::Assign(char *value)
{
    Type = PropertyType::STRING;
    strncpy(Value.String, value, sizeof(Value.String) - 1);
    Value.String[sizeof(Value.String) - 1] = '\0';
}

void TMProperty::Dump(int alignNameLen)
{
    cout << "  id=" << setw(3) << right << Id << ", name='" << Name << "', ";
    if (alignNameLen > 0)
        cout << setw(alignNameLen - Name.length()) << left << "";

    switch (Type)
    {
    case PropertyType::INT32:
        cout << "type=INT32,   value=" << Value.Int << endl;
        break;
    case PropertyType::UINT32:
        cout << "type=UINT32,  value=" << Value.Uint << endl;
        break;
    case PropertyType::BOOLEAN:
        cout << "type=BOOLEAN, value=" << (Value.Bool ? "True" : "False") << endl;
        break;
    case PropertyType::STRING:
        cout << "type=STRING,  value=" << Value.String << endl;
        break;
    default:
        cout << "Unknown type" << endl;
        break;
    }
}

int TMPropertyList::Add(TMProperty property)
{
    if (Find(property.Id))
    {
        return -1;
    }

    mProperties[property.Id] = property;
    return 0;
}

int TMPropertyList::Add(TMProperty *property)
{
    if (Find(property->Id))
    {
        return -1;
    }

    mProperties[property->Id] = *property;
    return 0;
}

void TMPropertyList::Reset()
{
    mProperties.clear();
}

int TMPropertyList::Count()
{
    return mProperties.size();
}

int TMPropertyList::FindDiff(vector<int> &diffIDArray, TMPropertyList *searchList)
{
    diffIDArray.clear();
    if (searchList == NULL)
    {
        cout << "propertyList is NULL" << endl;
        return TMResult::TM_EINVAL;
    }

    TMPropertyMap_t *searchMap = &(searchList->mProperties);

    for (auto iter = searchMap->begin(); iter != searchMap->end(); iter++)
    {
        int diffID;
        TMProperty *searchProp = &(iter->second);

        // find in this->mProperties
        auto find_iter = this->mProperties.find(searchProp->Id);

        if (find_iter == mProperties.end())
        {
            cout << "Can't find item which ID=" << searchProp->Id << ", dump below:" << endl;
            searchProp->Dump();
            diffID = searchProp->Id;
        }
        else
        {
            TMProperty *findProp = &(find_iter->second);

            switch (findProp->Type)
            {
            case TMProperty::PropertyType::INT32:
                if (findProp->Value.Int == searchProp->Value.Int)
                    continue;
                break;
            case TMProperty::PropertyType::UINT32:
                if (findProp->Value.Uint == searchProp->Value.Uint)
                    continue;
                break;
            case TMProperty::PropertyType::BOOLEAN:
                if (findProp->Value.Bool == searchProp->Value.Bool)
                    continue;
                break;
            case TMProperty::PropertyType::STRING:
                if (strncmp(findProp->Value.String, searchProp->Value.String, sizeof(findProp->Value.String)) == 0)
                    continue;
                break;
            default:
                cout << "Unknown type" << endl;
                continue;
                break;
            }

            diffID = searchProp->Id;
        }
        diffIDArray.push_back(diffID);
    }

    return diffIDArray.size();
}

bool TMPropertyList::Find(int Id)
{
    return (mProperties.find(Id) != mProperties.end());
}

void TMPropertyList::Dump()
{
    size_t maxNameLen = 0;
    TMProperty *property;
    for (auto iter = mProperties.begin(); iter != mProperties.end(); iter++)
    {
        property = &iter->second;
        if (property->Name.length() > maxNameLen)
        {
            maxNameLen = property->Name.length();
        }
    }

    for (auto iter = mProperties.begin(); iter != mProperties.end(); iter++)
    {
        property = &iter->second;
        property->Dump(maxNameLen);
    }
}

