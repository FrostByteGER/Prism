#pragma once

#include <memory>
#include "spdlog/spdlog.h"

namespace Prism::Utility::Logging
{
    class Log
    {
    public:
        std::shared_ptr<spdlog::logger> coreLogger;

        Log(const Log& other) = delete;
        Log(Log&& other) noexcept = delete;
        Log& operator=(const Log& other) = delete;
        Log& operator=(Log&& other) noexcept = delete;
        ~Log();

        void initialize();

        // instance generation and retrieval
        static Log& logInstance()
        {
            static Log instance;
            return instance;
        }

    private:
        Log()
        {
            initialize();
        }

        void shutdown();
    };
}

#if defined(LOG_CRITICAL) || defined(LOG_ERROR) || defined(LOG_WARN) || defined(LOG_INFO) || defined(LOG_DEBUG) || defined(LOG_TRACE)
    #error "Logging macros must not be previously defined!"
#else
#define LOG_CRITICAL(...) Prism::Utility::Logging::Log::logInstance().coreLogger->critical(__VA_ARGS__)
#define LOG_ERROR(...) Prism::Utility::Logging::Log::logInstance().coreLogger->error(__VA_ARGS__)
#define LOG_WARN(...) Prism::Utility::Logging::Log::logInstance().coreLogger->warn(__VA_ARGS__)
#define LOG_INFO(...) Prism::Utility::Logging::Log::logInstance().coreLogger->info(__VA_ARGS__)
#define LOG_DEBUG(...) Prism::Utility::Logging::Log::logInstance().coreLogger->debug(__VA_ARGS__)
#define LOG_TRACE(...) Prism::Utility::Logging::Log::logInstance().coreLogger->trace(__VA_ARGS__)
#endif
