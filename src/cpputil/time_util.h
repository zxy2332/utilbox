//
// Created by Administrator on 2023/9/8/008.
//

#pragma once

#include <chrono>
#include <string>
#include "cpputil_export.h"

namespace zxy::util{

    // 生成时间字符串
    // @format: 格式字符串
    // @tp: 时间点
    CPPUTIL_EXPORT std::string TimeString(const std::string& format,
                                  const std::chrono::system_clock::time_point& tp);

    // 生成时间字符串
    // 格式: “{:%Y-%m-%d %H:%M:%S}”
    // @tp: 时间点
    CPPUTIL_EXPORT std::string TimeString(const std::chrono::system_clock::time_point& tp);

    // 生成当前时间字符串
    // 格式: “{:%Y-%m-%d %H:%M:%S}”
    CPPUTIL_EXPORT std::string TimeStringNow() ;

    // 生成紧凑格式的时间字符串
    // 格式："{:%Y%m%d%H%M%S}"
    // @tp: 时间点
    CPPUTIL_EXPORT std::string CompactTimeString(const std::chrono::system_clock::time_point& tp);
};


