#pragma once
#include <chrono>

namespace Prism::Utility
{
    class StopWatch
    {
    public:
        void start();
        void stop();

        [[nodiscard]] double getTimeInSeconds() const;

        [[nodiscard]] double getTimeInMilliseconds() const;

        [[nodiscard]] double getTimeInMicroseconds() const;

        [[nodiscard]] uint64_t getTimeInNanoseconds() const;

    private:
        bool running = false;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
        std::chrono::duration<long long, std::ratio<1, 1000000000>> duration = std::chrono::duration<
            long long, std::ratio<1, 1000000000>>::zero();
    };
}
