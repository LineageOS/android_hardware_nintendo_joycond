#include "android_utils.h"

//public
std::string android_utils::property_get(std::string key)
{
    char value[PROPERTY_VALUE_MAX];

    ::property_get(key.c_str(), (char*)&value, "");

    return std::string(value);
}
