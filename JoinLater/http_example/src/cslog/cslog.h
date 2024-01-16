#ifndef CS_SIM_LOG_H
#define CS_SIM_LOG_H

#include <set>
#include <string>
#include <mutex>
#include <sstream>
#include <iostream>

#include "cslog_export.h"

extern "C" {
  bool CSLOG_EXPORT CreateLog(const char* log_path);
}

#define Log(category, level, msg) \
CSLog::Instance()->Write(category, level, msg, __FILE__, __LINE__, __FUNCTION__);


#define Log_Manager     CSLog::Instance
#define LOG_TRACE       CMessageLogger(__FILE__, __LINE__, __FUNCTION__).trace
#define LOG_DEBUG       CMessageLogger(__FILE__, __LINE__, __FUNCTION__).debug
#define LOG_INFO        CMessageLogger(__FILE__, __LINE__, __FUNCTION__).info
#define LOG_WARNING     CMessageLogger(__FILE__, __LINE__, __FUNCTION__).warning
#define LOG_ERROR       CMessageLogger(__FILE__, __LINE__, __FUNCTION__).error
#define LOG_CRITICAL    CMessageLogger(__FILE__, __LINE__, __FUNCTION__).critical
#define LOG_OFF         CMessageLogger(__FILE__, __LINE__, __FUNCTION__).off


enum class OutputType {
  // 控制台输出
  CONSOLE_OUT = 0,
  // 文件输出
  FILE_OUT = 1,
  // 控制台和文件同时输出
  CONSOLE_FILE_OUT = 2,
  // 上传服务器
  SERVER_OUT = 3
};

// 日志类
class CSLOG_EXPORT CSLog {
 public:
  static CSLog* Instance();

  bool CreateLog(const std::string& log_path);
  bool CreateLog();
  bool CreateModuleLog(const std::string& module);

  void SetLevel(uint32_t level);
  void SetLevel(const std::string &level);
  uint32_t GetLevel();

  // 配置日志输出类型
  void SetLogOutputType(int type);

  bool AddCategory(const std::string&);

  void Write(const std::string& name, uint32_t level, const std::string& msg,
             const char *filename_in, int line_in, const char *funcname_in);
  void Write(const std::string& category, uint32_t level,
             const std::string& msg);

  bool CloseLog();

  void SetCurrentTime(int current_time);
  void SetCurrentTime(const std::string& current_time);
  // 配置日志服务器ip和端口，调用SetLogOutputType启用上传服务器模式
  void SetServerConfig(std::string & host, int port);

  // 控制台传入设置的等级信息
  void SetLogLevels(const std::set<uint32_t> &levels);

 private:
  CSLog() = default;
  ~CSLog() = default;
  CSLog(const CSLog&) = delete;
  CSLog& operator=(const CSLog&) = delete;

 private:
  std::string log_path_;
  int current_time_ = 0;
  std::string current_time_str_ = "0.000000";

  std::set<std::string> log_names_;
  //表示当前日志输出地类型
  OutputType output_type_=OutputType::CONSOLE_FILE_OUT;
  uint32_t level_ = 0;
  std::mutex mx_;
  //日志Server的ip和端口
  std::string host_;
  int port_;

  //日志的等级
  std::set<uint32_t> levels_;
};


class CLogHelp :public std::stringstream
{
private:
    uint32_t m_level;
    const char* m_fun;
    int m_line;
    const char* m_file;
public:
    CLogHelp(uint32_t level) :m_level(level) {}
    CLogHelp(uint32_t level, const char *filename_in, int line_in, const char *funcname_in) :
        m_level(level), m_file(filename_in), m_line(line_in), m_fun(funcname_in){}
    ~CLogHelp()
    {
        CSLog::Instance()->Write("Test", m_level,this->str(), m_file, m_line, m_fun);
    }

#if 0   //TODO 待时间充裕时增加QT类型
#ifdef QT_VERSION
#include <QString>
    CLogHelp&operator<<(const QString &t)
    {
        return*this;
    }

    CLogHelp&operator<<(qint64 t)
    {
        return*this;

    }

    CLogHelp&operator<<(quint64 t)
    {
        return*this;
    }

    CLogHelp&operator<<(const QByteArray &t)
    {
        return*this;
    }

#endif // QT_VERSION
#endif

};

class CMessageLogger
{
    const char* m_fun;
    int m_line;
    const char* m_file;
public:
    CMessageLogger() {}
    CMessageLogger(const char *filename_in, int line_in, const char *funcname_in) :
        m_file(filename_in), m_line(line_in), m_fun(funcname_in){}

    inline CLogHelp trace() { return CLogHelp(0, m_file, m_line, m_fun); }
    inline CLogHelp debug() { return CLogHelp(1,m_file,m_line, m_fun); }
    inline CLogHelp info() { return CLogHelp(2, m_file, m_line, m_fun); }
    inline CLogHelp warning() { return CLogHelp(3, m_file, m_line, m_fun); }
    inline CLogHelp error() { return CLogHelp(4, m_file, m_line, m_fun); }
    inline CLogHelp critical() { return CLogHelp(5, m_file, m_line, m_fun); }
    inline CLogHelp off() { return CLogHelp(6, m_file, m_line, m_fun); }
};

#endif