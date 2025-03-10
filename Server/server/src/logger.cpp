#include "logger.hpp"

namespace btyGoose{
std::shared_ptr<spdlog::logger> g_default_logger;
void init_logger(bool mode, const std::string &file, int32_t level)
{
    if (mode == false) {
        //如果是调试模式，则创建标准输出日志器，输出等级为最低
        g_default_logger = spdlog::stdout_color_mt("default-logger");
        g_default_logger->set_level(spdlog::level::level_enum::trace);
        g_default_logger->flush_on(spdlog::level::level_enum::trace);
        g_default_logger->set_pattern("[%n][%H:%M:%S][%t]%^[%-8l]%$%v");//标记颜色的起始位置
    }else {
        //否则是发布模式，则创建文件输出日志器，输出等级根据参数而定
        g_default_logger = spdlog::basic_logger_mt("default-logger", file);
        g_default_logger->set_level((spdlog::level::level_enum)level);
        g_default_logger->flush_on((spdlog::level::level_enum)level);
        g_default_logger->set_pattern("[%n][%H:%M:%S][%t][%-8l]%v");
    }
    
}
}