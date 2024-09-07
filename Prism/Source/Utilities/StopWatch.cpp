#include "StopWatch.hpp"

namespace Prism::Utility
{
    void StopWatch::start()
    {
        startTime = std::chrono::high_resolution_clock::now();
        running = true;
    }

    void StopWatch::stop()
    {
        if (!running)
        {
            return;
        }
        endTime = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    }

    [[nodiscard]] double StopWatch::getTimeInSeconds() const
    {
        return static_cast<double>(duration.count()) / 1000000000.0;
    }

    [[nodiscard]] double StopWatch::getTimeInMilliseconds() const
    {
        return static_cast<double>(duration.count()) / 1000000.0;
    }

    [[nodiscard]] double StopWatch::getTimeInMicroseconds() const
    {
        return static_cast<double>(duration.count()) / 1000.0;
    }

    [[nodiscard]] uint64_t StopWatch::getTimeInNanoseconds() const
    {
        return duration.count();
    }
}
