#include "cslog.h"

#include <filesystem>
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
#include <locale>
#include <codecvt>
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/tcp_sink.h"
#include "spdlog/async.h"

#define SERVER_LOG "server"

std::string GetNowStr(){
    std::time_t t = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
#ifdef _MSC_VER
    std::tm tm2;
    localtime_s(&tm2, &t);
    char buf[100] = { 0 };
    std::strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", &tm2);
#elif defined(__GNUC__)
    struct tm now_time;
    localtime_r(&t, &now_time);
    char buf[100] = { 0 };
    std::strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", &now_time);
#endif
    return buf;
}
namespace spdlog {
template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> tcp_logger_mt(const std::string& logger_name,
    sinks::tcp_sink_config config) {
  return Factory::template create<sinks::tcp_sink_mt>(logger_name,config);
}

template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> tcp_logger_st(const std::string& logger_name,
    sinks::tcp_sink_config config) {
  return Factory::template create<sinks::tcp_sink_st>(logger_name,config);
}
}

CSLog* CSLog::Instance() {
  static CSLog inst;
  return &inst;
}

bool CSLog::CreateLog(const std::string& log_path) {
  spdlog::set_level(spdlog::level::trace);
  spdlog::flush_every(std::chrono::seconds(2));
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%s:%#] [%l] %v");
  log_path_ = log_path;
  level_ = spdlog::level::trace;
  bool res = AddCategory("log");
  return res;
}

bool CSLog::CreateLog()
{
    std::string str_log_path;
    str_log_path.append(std::filesystem::current_path().string())
        .append("/logs/").append(GetNowStr()).append(".txt");

    return this->CreateLog(str_log_path);
}

bool CSLog::CreateModuleLog(const std::string& module)
{
    std::string str_log_path;
    str_log_path.append(std::filesystem::current_path().string())
        .append("/logs/").append(module).append("/").append(GetNowStr()).append(".txt");

    return this->CreateLog(str_log_path);
}

void CSLog::SetLevel(uint32_t level) {
  spdlog::set_level(spdlog::level::level_enum(level));
  level_ = level;
}

void CSLog::SetLogOutputType(int type) {
  output_type_ = OutputType(type);
}

void CSLog::SetLevel(const std::string &level) {
  if (level == "trace") {
    SetLevel(0);
  } else if (level == "debug") {
    SetLevel(1);
  } else if (level == "info") {
    SetLevel(2);
  } else if (level == "warning") {
    SetLevel(3);
  } else if (level == "error") {
    SetLevel(4);
  } else if (level == "critical") {
    SetLevel(5);
  } else if (level == "off") {
    SetLevel(6);
  }
}

uint32_t CSLog::GetLevel() {
  return level_;
}

bool CSLog::AddCategory(const std::string& category) {
  std::lock_guard lk(mx_);
  auto log = spdlog::get(category);
  if (log) {
    return true;
  }

  //根据日志输出类型 设定logger
  switch (output_type_) {
  case OutputType::CONSOLE_OUT: {
    auto logger = spdlog::stdout_color_mt(category);
    if (logger) {
      log_names_.insert(category);
      logger->info("create log category success");
      return true;
    }
    break;
  }
  case OutputType::FILE_OUT: {
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    auto logger = spdlog::basic_logger_mt<spdlog::async_factory>(category,
                  converter.from_bytes(log_path_));
#else
    auto logger = spdlog::basic_logger_mt<spdlog::async_factory>(category,
                  (log_path_));
#endif
    if (logger) {
      log_names_.insert(category);
      logger->info("create log category success");
      return true;
    }
    break;
  }
  case OutputType::CONSOLE_FILE_OUT: {
    auto logger = spdlog::stdout_color_mt(category);
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    auto logger2 = spdlog::basic_logger_mt<spdlog::async_factory>(category + "1",
                   converter.from_bytes(log_path_));
#else
    auto logger2 = spdlog::basic_logger_mt<spdlog::async_factory>(category + "1",
                   (log_path_));
#endif
    if (logger && logger2) {
      log_names_.insert(category);
      logger->info("create log category success");
      logger2->info("create log category success");
      return true;
    }
    break;
  }
  case OutputType::SERVER_OUT: {
    // 生成一个tcp_logger对象，配置上传目的地的ip和端口
    spdlog::sinks::tcp_sink_config config(host_, port_);
    auto logger = spdlog::tcp_logger_mt(category, config);
    if (logger) {
      log_names_.insert(category);
      logger->info("create log category success");
      return true;
    }
  }
  }
  return false;
}

bool CSLog::CloseLog() {
  auto lg = spdlog::get("log");
  if (lg) {
    lg->info("log exit");
  }
  //退出前输出所有日志
  for (auto& p : log_names_) {
    auto log = spdlog::get(p);
    if (log) {
      log->flush();
    }
  }
  spdlog::shutdown();
  return true;
}

void CSLog::SetCurrentTime(int current_time) {
  current_time_ = current_time;
}

void CSLog::SetCurrentTime(const std::string& current_time) {
  current_time_str_ = current_time;
}

void CSLog::SetServerConfig(std::string & host, int port) {
  host_ = host;
  port_ = port;
}

void CSLog::SetLogLevels(const std::set<uint32_t> &levels) {
  levels_ = levels;
}

void CSLog::Write(const std::string& name, uint32_t level,
                     const std::string& msg, const char *filename_in, int line_in,
                     const char *funcname_in) {
  // 判断等级是否为控制台想要输出的
  if (auto it = levels_.find(level); it == levels_.end()) {
    if (!levels_.empty()) {
      return;
    }
  }

  // 日志上传服务器特殊处理
  // 多个category会开多个tcp client
  // 单个tcp client不占用过多资源
  if (output_type_ == OutputType::SERVER_OUT) {
    spdlog::source_loc sl(filename_in, line_in, funcname_in);
    std::shared_ptr<spdlog::logger> lg = spdlog::get(SERVER_LOG);
    if (!lg) {
      AddCategory(SERVER_LOG);
    }
    auto logger = spdlog::get(SERVER_LOG);
    if (!logger) {
      return;
    }

    // 具体的category追加在时间信息和msg中间
    std::string time_msg = std::string("[") + current_time_str_ + std::string("] ")
                           + std::string("[") + name + std::string("] ")
                           + msg;
    logger->log(sl, spdlog::level::level_enum(level), "{0}", time_msg);
  } else {
    spdlog::source_loc sl(filename_in, line_in, funcname_in);
    std::shared_ptr<spdlog::logger> lg = spdlog::get(name);
    if (!lg) {
      AddCategory(name);
    }
    auto logger = spdlog::get(name);
    if (!logger) {
      return;
    }
    std::string time_msg = std::string("[") + current_time_str_ + std::string("] ")
                           + msg;
    if (output_type_ == OutputType::CONSOLE_FILE_OUT) {
      auto logger2 = spdlog::get(name + "1");
      logger->log(sl, spdlog::level::level_enum(level), "{0}", time_msg);
      logger2->log(sl, spdlog::level::level_enum(level), "{0}", time_msg);
    } else {
      logger->log(sl, spdlog::level::level_enum(level), "{0}", time_msg);
    }
  }
}

void CSLog::Write(const std::string& category, uint32_t level,
                     const std::string& msg) {
  // 判断等级是否为控制台想要输出的
  if (auto it = levels_.find(level); it == levels_.end()) {
    if (!levels_.empty()) {
      return;
    }
  }

  // 上传服务器模式，只用单个tcp client
  if (output_type_==OutputType::SERVER_OUT) {
    std::shared_ptr<spdlog::logger> lg = spdlog::get(SERVER_LOG);
    if (!lg) {
      AddCategory(SERVER_LOG);
    }
    auto logger = spdlog::get(SERVER_LOG);
    if (!logger) {
      return;
    }
    // category追加在时间信息和msg中间
    std::string time_msg = std::string("[") + current_time_str_ + std::string("] ")
                           + std::string("[") + category + std::string("] ")
                           + msg;
    logger->log(spdlog::level::level_enum(level), "{0}", time_msg);
  } else {
    std::shared_ptr<spdlog::logger> lg = spdlog::get(category);
    if (!lg) {
      AddCategory(category);
    }
    auto logger = spdlog::get(category);
    if (!logger) {
      return;
    }
    std::string time_msg = std::string("[") + current_time_str_ + std::string("] ")
                           + msg;
    logger->log(spdlog::level::level_enum(level), "{0}", time_msg);
  }
}

bool CreateLog(const char* log_path) {
  return CSLog::Instance()->CreateLog(log_path);
}
