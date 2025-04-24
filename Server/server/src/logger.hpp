#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <iostream>

namespace btyGoose{
extern std::shared_ptr<spdlog::logger> g_default_logger;
void init_logger(bool mode, const std::string &file, int32_t level);

#define SUP_LOG_TRACE(format, ...) btyGoose::g_default_logger->trace(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_DEBUG(format, ...) btyGoose::g_default_logger->debug(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_INFO(format, ...) btyGoose::g_default_logger->info(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_WARN(format, ...) btyGoose::g_default_logger->warn(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_ERROR(format, ...) btyGoose::g_default_logger->error(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SUP_LOG_FATAL(format, ...) btyGoose::g_default_logger->critical(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)


class ScopedTimer
{
public:
    ScopedTimer(const std::string&name)
    :_name(name),_start(std::chrono::high_resolution_clock::now())
    {
        _prev = _start;
    }

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