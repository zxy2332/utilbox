#ifndef SHORTEST_UTIL_H
#define SHORTEST_UTIL_H
#include "shortestescape_export.h"
#include <string>

#define TimeBegin(x) auto x = util::SteadyNow();
#define TimeEnd(s,x) if(s){LOG_DEBUG()<<#x<< " calculation time: " << util::ElapsedTimeUS(x,util::SteadyNow()) <<" us" ;}

namespace util {
    //写文件
    SHORTESTESCAPE_EXPORT bool write_file(const std::string &file_name,const std::string &file_content,bool is_u8 = false);
    //判断文件是否存在
    SHORTESTESCAPE_EXPORT bool IsExist(const std::string &file_name,bool is_u8 = false);
    //读取文件内容
    SHORTESTESCAPE_EXPORT std::string file_contents(const std::string &file_name,bool is_u8 = false);
    //string转wstring
    SHORTESTESCAPE_EXPORT std::wstring string2wstring(const std::string &str);
    //wstring转string
    SHORTESTESCAPE_EXPORT std::string wstring2string(const std::wstring &str);
    //根据ExceptionEnable控制是否抛出异常
    SHORTESTESCAPE_EXPORT void Throw(const std::exception &e);
    //SHORTESTESCAPE_EXPORT void Throw(const std::exception &e);
    SHORTESTESCAPE_EXPORT void Throw(const std::string &msg);

    //获取当前系统时间 nas
    SHORTESTESCAPE_EXPORT uint64_t SystemNow();
    //获取当前时钟时间 nas
    SHORTESTESCAPE_EXPORT uint64_t SteadyNow();
    //时间测试用
    //计算begin到end消耗的时间
    SHORTESTESCAPE_EXPORT double ElapsedTimeS(uint64_t begin,uint64_t end);
    SHORTESTESCAPE_EXPORT double ElapsedTimeMS(uint64_t begin,uint64_t end);
    SHORTESTESCAPE_EXPORT double ElapsedTimeUS(uint64_t begin,uint64_t end);
};
#endif