//
// Created by Administrator on 2023/9/8/008.
//

#include "time_util.h"

#include "fmt/format.h"
#include "fmt/chrono.h"


namespace zxy::util{

    // 生成时间字符串
    // @format: 格式字符串
    // @tp: 时间点
    std::string TimeString(const std::string& format,
                                  const std::chrono::system_clock::time_point& tp) {
        return fmt::format(format,
                           fmt::localtime(std::chrono::system_clock::to_time_t(tp)));
    }

    // 生成时间字符串
    // 格式: “{:%Y-%m-%d %H:%M:%S}”
    // @tp: 时间点
    std::string TimeString(const std::chrono::system_clock::time_point& tp) {
        return util::TimeString("{:%Y-%m-%d %H:%M:%S}", tp);
    }

    // 生成当前时间字符串
    // 格式: “{:%Y-%m-%d %H:%M:%S}”
    std::string TimeStringNow() {
        return TimeString(std::chrono::system_clock::now());
    }

    // 生成紧凑格式的时间字符串
    // 格式："{:%Y%m%d%H%M%S}"
    // @tp: 时间点
    std::string CompactTimeString(const
                                         std::chrono::system_clock::time_point& tp) {
        return util::TimeString("{:%Y%m%d%H%M%S}", tp);
    }
};
