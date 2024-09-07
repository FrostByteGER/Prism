#include "Log.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace Prism::Utility::Logging
{
    Log::~Log()
    {
        shutdown();
    }

    void Log::initialize()
    {
        const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        const auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Prism.log");
        spdlog::sinks_init_list sinks = {consoleSink, fileSink};
        coreLogger = std::make_shared<spdlog::logger>("Prism", sinks);
        coreLogger->flush_on(spdlog::level::err);
#ifdef Prism_DEBUG
        coreLogger->set_level(spdlog::level::trace);
#else
		coreLogger->set_level(spdlog::level::warn);
#endif
    }

    void Log::shutdown()
    {
        coreLogger->flush();
    }
}
