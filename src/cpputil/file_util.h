#pragma once
#include "cpputil_export.h"
#include <string>
namespace zxy::util {
    //写文件
    CPPUTIL_EXPORT bool write_file(const std::string &file_name,const std::string &file_content);
    //读取文件内容
    CPPUTIL_EXPORT std::string file_contents(const std::string &file_name);
    //string转wstring
    CPPUTIL_EXPORT std::wstring string2wstring(const std::string &str);
    //wstring转string
    CPPUTIL_EXPORT std::string wstring2string(const std::wstring &str);
};
