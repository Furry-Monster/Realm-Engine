#pragma once

#include <cstdint>
#include <memory>

#include <spdlog/logger.h>
#include <stdexcept>

#define LOG_HELPER(LOG_LEVEL, ...) \
    g_context.m_logger->log(LOG_LEVEL, "[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_DEBUG(...) LOG_HELPER(Logger::LogLevel::debug, __VA_ARGS__)
#define LOG_INFO(...) LOG_HELPER(Logger::LogLevel::info, __VA_ARGS__)
#define LOG_WARN(...) LOG_HELPER(Logger::LogLevel::warn, __VA_ARGS__)
#define LOG_ERROR(...) LOG_HELPER(Logger::LogLevel::error, __VA_ARGS__)
#define LOG_FATAL(...) LOG_HELPER(Logger::LogLevel::fatal, __VA_ARGS__)

class Logger final
{
public:
    enum class LogLevel : uint8_t
    {
        debug,
        info,
        warn,
        error,
        fatal,
    };

    Logger();
    ~Logger();

    template<typename... TARGS>
    void log(const LogLevel level, TARGS&&... args) const
    {
        switch (level)
        {
            case LogLevel::debug:
                m_logger->debug(std::forward<TARGS>(args)...);
                break;
            case LogLevel::info:
                m_logger->info(std::forward<TARGS>(args)...);
                break;
            case LogLevel::warn:
                m_logger->warn(std::forward<TARGS>(args)...);
                break;
            case LogLevel::error:
                m_logger->error(std::forward<TARGS>(args)...);
                break;
            case LogLevel::fatal:
                m_logger->critical(std::forward<TARGS>(args)...);
                const std::string fmt_str = fmt::format(std::forward<TARGS>(args)...);
                throw std::runtime_error(fmt_str);
                break;
        }
    }

private:
    std::shared_ptr<spdlog::logger> m_logger;
};