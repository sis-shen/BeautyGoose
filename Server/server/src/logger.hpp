/**
 * @file logger.hpp
 * @author supdriver
 * @date 2025-05-26
 * @brief spdlog日志器二次封装
 * @version 2.6.0
 */

#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <iostream>

namespace btyGoose{
extern std::shared_ptr<spdlog::logger> g_default_logger;//声明全局日志器

/**
 * @brief 初始化全局日志器
 * @param debug_mode 调试模式，true: debug模式标准输出 false: release模式文件输出
 * @param file 输出的日志文件
 * @param level 输出的日志等级
 * @return 是否添加成功
 */
void init_logger(bool debug_mode, const std::string &file, int32_t level);

#define SUP_LOG_TRACE(format, ...) btyGoose::g_default_logger->trace(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_DEBUG(format, ...) btyGoose::g_default_logger->debug(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_INFO(format, ...) btyGoose::g_default_logger->info(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_WARN(format, ...) btyGoose::g_default_logger->warn(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_ERROR(format, ...) btyGoose::g_default_logger->error(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_FATAL(format, ...) btyGoose::g_default_logger->critical(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * @class ScopedTimer
 * @brief 计时器类
 * @details 每一次调用接口都会记录一次时间，类似秒表
 */
class ScopedTimer
{
public:
    ScopedTimer(const std::string&name)
    :_name(name),_start(std::chrono::high_resolution_clock::now())
    {
        _prev = _start;
    }

    /**
     * @brief 打表函数，记录一次时间并返回时间间隔
     * @return 距离上次调用经过的毫秒数
     */
    int64_t staged()
    {
        auto stage = std::chrono::high_resolution_clock::now();
        auto ret = stage - _prev;
        _prev = stage;
        return (std::chrono::duration_cast<std::chrono::microseconds>(ret)).count();
    }
        
    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-_start);
        SUP_LOG_DEBUG("计时器 {} 存活时间: {} μs",_name,duration.count());
    }

private:
    std::string _name;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    std::chrono::time_point<std::chrono::high_resolution_clock> _prev;
};
}