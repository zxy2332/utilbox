#include "file_util.h"
#include <fstream>
#include <locale>
#include <codecvt>
#include <filesystem>

#include <iostream>

namespace zxy::util {
    std::string CPPUTIL_EXPORT file_contents(const std::string &file_name) {
        std::ifstream ifs(std::filesystem::u8path(file_name), std::ios_base::binary);
        if (!ifs) {
            return "";
        }
        return std::string(std::istreambuf_iterator<char>(ifs),std::istreambuf_iterator<char>());
    }

    std::wstring util::string2wstring(const std::string &str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    std::string util::wstring2string(const std::wstring &str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(str);
    }

    bool write_file(const std::string &file_name, const std::string &file_content) {
        std::ofstream ofs(std::filesystem::u8path(file_name));
        if(ofs){
            ofs << file_content;
            ofs.close();
            return true;
        }
        return false;
    }
};