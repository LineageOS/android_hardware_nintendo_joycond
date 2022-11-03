#ifndef JOYCOND_ANDROID_UTILS_H
#define JOYCOND_ANDROID_UTILS_H

#include "cutils/properties.h"

#include <string>

class android_utils {
    public:
        static std::string property_get(std::string key);
        
};

#endif
