#ifndef SHORTESTPATH_HTTPTEST_H
#define SHORTESTPATH_HTTPTEST_H
#include <string>
#include <filesystem>
#include <fstream>
#include "cpp-httplib/httplib.h"

struct HttpTestPrivate;
class HttpTest {
public:
    HttpTest();
    ~HttpTest();
    //连接
    bool Conncet(const std::string &url);


    httplib::Result CalculateShortest(const std::string &body);

    httplib::Result RefreshPathData();

    httplib::Result InitPathData(const std::string &body);
private:
    HttpTestPrivate *d_= nullptr;
};

inline std::filesystem::path ProcessCode(const std::string &file_name,bool is_u8){
    if (is_u8){
        //通过u8path 转为 path
        return std::filesystem::u8path(file_name).generic_string();
    }else {
        //无需处理
        return file_name;
    }
}

inline bool IsExist(const std::string &file_name,bool is_u8 = false) {
    auto path = ProcessCode(file_name,is_u8);
    if (!std::filesystem::is_regular_file(path)){
        return false;
    }
    return true;
}
inline std::string  file_contents(const std::string &file_name,bool is_u8 = false) {
    auto path = ProcessCode(file_name,is_u8);
    if(!IsExist(file_name)){
        throw (std::runtime_error("File does not exist. Path:"+path.generic_string()));
        return "";
    }
    std::ifstream ifs(path, std::ios_base::binary);
    if (!ifs) {
        throw (std::runtime_error("File opening failure. Path:"+path.generic_string()));
        return "";
    }
    return std::string(std::istreambuf_iterator<char>(ifs),std::istreambuf_iterator<char>());
}

#endif //SHORTESTPATH_HTTPSERVER_H
