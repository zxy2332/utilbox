#include "util.h"
#include <fstream>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <chrono>
#include "cslog.h"

#include <iostream>

namespace util {

    std::filesystem::path ProcessCode(const std::string &file_name,bool is_u8){
        if (is_u8){
            //通过u8path 转为 path
            return std::filesystem::u8path(file_name).generic_string();
        }else {
            //无需处理
            return file_name;
        }
    }

    bool IsExist(const std::string &file_name,bool is_u8) {
        auto path = ProcessCode(file_name,is_u8);
        if (!std::filesystem::is_regular_file(path)){
            return false;
        }
        return true;
    }

    std::string  file_contents(const std::string &file_name,bool is_u8) {
        auto path = ProcessCode(file_name,is_u8);
        if(!IsExist(file_name)){
            Throw(std::runtime_error("File does not exist. Path:"+path.generic_string()));
            return "";
        }
        std::ifstream ifs(path, std::ios_base::binary);
        if (!ifs) {
            Throw(std::runtime_error("File opening failure. Path:"+path.generic_string()));
            return "";
        }
        return std::string(std::istreambuf_iterator<char>(ifs),std::istreambuf_iterator<char>());
    }

    std::wstring string2wstring(const std::string &str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    std::string wstring2string(const std::wstring &str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(str);
    }

    bool write_file(const std::string &file_name, const std::string &file_content,bool is_u8) {
        auto path = ProcessCode(file_name,is_u8);
        if (!std::filesystem::exists(path.parent_path())){
            std::filesystem::create_directories(path.parent_path());
        }
        std::ofstream ofs(path);
        if(ofs){
            ofs << file_content;
            ofs.close();
            return true;
        }
        return false;
    }

    void Throw(const std::exception &e) {
        Throw(e.what());

    }

    void Throw(const std::string &msg) {
        LOG_ERROR()<<msg;
#ifdef ExceptionEnable
        throw std::runtime_error(msg);
#endif
    }

    uint64_t SystemNow() {
        return std::chrono::system_clock::now().time_since_epoch().count();
    }

    uint64_t SteadyNow() {
        return std::chrono::steady_clock::now().time_since_epoch().count();
    }

    double ElapsedTimeS(uint64_t begin, uint64_t end) {
        return (double(end-begin))/1e9;
        std::chrono::nanoseconds d(end-begin);
        return std::chrono::duration_cast<std::chrono::seconds>(d).count();
    }

    double ElapsedTimeMS(uint64_t begin, uint64_t end) {
        return (double(end-begin))/1e6;
        std::chrono::nanoseconds d(end-begin);
        return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
    }

    double ElapsedTimeUS(uint64_t begin, uint64_t end) {
        return (double(end-begin))/1e3;
        std::chrono::nanoseconds d(end-begin);
        return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
    }

};