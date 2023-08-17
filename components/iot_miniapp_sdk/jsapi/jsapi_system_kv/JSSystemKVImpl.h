#pragma once
#include <string>
#include <stdio.h>
#include "aos/kv.h"
#include "jqutil_v2/jqutil.h"

using namespace jqutil_v2;

namespace aiot
{

class JSSystemKVImpl
{
public:
    void setItem(std::string key, std::string value);
    std::string getItem(std::string key, std::string defaultValue);
    void removeItem(std::string key);
    void clear();
};
}  // namespace aiot
