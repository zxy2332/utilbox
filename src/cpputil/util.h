#pragma once
#include <vector>
#include <string>
#include "cpputil_export.h"

namespace zxy::util {
//字符串分割函数
    CPPUTIL_EXPORT std::vector<std::string> split(std::string str, std::string pattern);
};

