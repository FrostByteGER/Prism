#pragma once

#include <chrono>

namespace Prism::Utility
{
    class EngineClock
    {
    public:
        EngineClock();

        [[nodiscard]] uint64_t engineElapsedSeconds() const;
        [[nodiscard]] uint64_t engineElapsedMilliseconds() const;
        [[nodiscard]] uint64_t engineElapsedMicroseconds() const;
        [[nodiscard]] uint64_t engineElapsedNanoseconds() const;

        /*[[nodiscard]] float physicsAverageSeconds() const;
        [[nodiscard]] long double physicsAverageSecondsDouble() const;
        [[nodiscard]] float updateAverageSeconds() const;
        [[nodiscard]] long double updateAverageSecondsDouble() const;
        [[nodiscard]] float renderAverageSeconds() const;
        [[nodiscard]] long double renderAverageSecondsDouble() const;
        [[nodiscard]] float physicsAverageMs() const;
        [[nodiscard]] long double physicsAverageMsDouble() const;
        [[nodiscard]] float updateAverageMs() const;
        [[nodiscard]] long double updateAverageMsDouble() const;
        [[nodiscard]] float renderAverageMs() const;
        [[nodiscard]] long double renderAverageMsDouble() const;*/

        void startPhysicsTimer();
        void stopPhysicsTimer();
        void startUpdateTimer();
        void stopUpdateTimer();
        void startRenderTimer();
        void stopRenderTimer();

        void updateFrameDelta();
        [[nodiscard]] float getFrameDelta() const;
        [[nodiscard]] float getFrameDeltaMilliseconds() const;

        [[nodiscard]] uint32_t getFrameCount() const;

        [[nodiscard]] float getPhysicsDelta() const;
        [[nodiscard]] float getPhysicsDeltaMilliseconds() const;
        [[nodiscard]] float getUpdateDelta() const;
        [[nodiscard]] float getUpdateDeltaMilliseconds() const;
        [[nodiscard]] float getRenderDelta() const;
        [[nodiscard]] float getRenderDeltaMilliseconds() const;

        void reset();

    private:
        uint32_t frameCount = 0;

        std::chrono::time_point<std::chrono::high_resolution_clock> engineStartupTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;

        bool physicsTimerStarted = false;
        bool updateTimerStarted = false;
        bool renderTimerStarted = false;

        std::chrono::time_point<std::chrono::high_resolution_clock> startPhysicsTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> startUpdateTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> startRenderTime;

        std::chrono::time_point<std::chrono::high_resolution_clock> endPhysicsTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> endUpdateTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> endRenderTime;

        std::chrono::duration<long long, std::ratio<1, 1000000>> frameDelta = std::chrono::duration<
            long long, std::ratio<1, 1000000>>::zero();
        std::chrono::duration<long long, std::ratio<1, 1000000000>> physicsTime = std::chrono::duration<
            long long, std::ratio<1, 1000000000>>::zero();
        std::chrono::duration<long long, std::ratio<1, 1000000000>> updateTime = std::chrono::duration<
            long long, std::ratio<1, 1000000000>>::zero();
        std::chrono::duration<long long, std::ratio<1, 1000000000>> renderTime = std::chrono::duration<
            long long, std::ratio<1, 1000000000>>::zero();
    };
}
