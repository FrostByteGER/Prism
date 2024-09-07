#include "EngineClock.hpp"
#include "Logging/Log.hpp"

Prism::Utility::EngineClock::EngineClock()
{
    const auto now = std::chrono::high_resolution_clock::now();
    engineStartupTime = now;
    lastFrameTime = now;

    startPhysicsTime = now;
    startRenderTime = now;
    startUpdateTime = now;

    endPhysicsTime = startPhysicsTime;
    endRenderTime = startRenderTime;
    endUpdateTime = startUpdateTime;
}

uint64_t Prism::Utility::EngineClock::engineElapsedSeconds() const
{
    const auto now = std::chrono::high_resolution_clock::now();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - engineStartupTime);
    const uint64_t delta = std::chrono::duration_cast<std::chrono::duration<uint64_t>>(seconds).count();
    return delta;
}

uint64_t Prism::Utility::EngineClock::engineElapsedMilliseconds() const
{
    const auto now = std::chrono::high_resolution_clock::now();
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - engineStartupTime);
    const uint64_t delta = std::chrono::duration_cast<std::chrono::duration<uint64_t>>(milliseconds).count();
    return delta;
}

uint64_t Prism::Utility::EngineClock::engineElapsedMicroseconds() const
{
    const auto now = std::chrono::high_resolution_clock::now();
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - engineStartupTime);
    const uint64_t delta = std::chrono::duration_cast<std::chrono::duration<uint64_t>>(microseconds).count();
    return delta;
}

uint64_t Prism::Utility::EngineClock::engineElapsedNanoseconds() const
{
    const auto now = std::chrono::high_resolution_clock::now();
    const auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now - engineStartupTime);
    const uint64_t delta = std::chrono::duration_cast<std::chrono::duration<uint64_t>>(nanoseconds).count();
    return delta;
}

void Prism::Utility::EngineClock::startPhysicsTimer()
{
    startPhysicsTime = std::chrono::high_resolution_clock::now();
    physicsTimerStarted = true;
}

void Prism::Utility::EngineClock::stopPhysicsTimer()
{
    if (!physicsTimerStarted)
    {
        return;
    }
    physicsTimerStarted = false;
    endPhysicsTime = std::chrono::high_resolution_clock::now();
    physicsTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endPhysicsTime - startPhysicsTime);
}

void Prism::Utility::EngineClock::startUpdateTimer()
{
    startUpdateTime = std::chrono::high_resolution_clock::now();
    updateTimerStarted = true;
}

void Prism::Utility::EngineClock::stopUpdateTimer()
{
    if (!updateTimerStarted)
    {
        return;
    }
    updateTimerStarted = false;
    endUpdateTime = std::chrono::high_resolution_clock::now();
    updateTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endUpdateTime - startUpdateTime);
}

void Prism::Utility::EngineClock::startRenderTimer()
{
    startRenderTime = std::chrono::high_resolution_clock::now();
    renderTimerStarted = true;
}

void Prism::Utility::EngineClock::stopRenderTimer()
{
    if (!renderTimerStarted)
    {
        return;
    }
    renderTimerStarted = false;
    endRenderTime = std::chrono::high_resolution_clock::now();
    renderTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endRenderTime - startRenderTime);
}

void Prism::Utility::EngineClock::updateFrameDelta()
{
    ++frameCount;
    const auto now = std::chrono::high_resolution_clock::now();
    frameDelta = std::chrono::duration_cast<std::chrono::microseconds>(now - lastFrameTime);
    lastFrameTime = now;
}


///
/// Returns Frame Delta in Seconds
///
float Prism::Utility::EngineClock::getFrameDelta() const
{
    const float deltaSeconds = std::chrono::duration_cast<std::chrono::duration<float>>(frameDelta).count();
    return deltaSeconds;
}

///
/// Returns Frame Delta in Milliseconds
///
float Prism::Utility::EngineClock::getFrameDeltaMilliseconds() const
{
    return static_cast<float>(frameDelta.count()) / 1000.0f;
}

uint32_t Prism::Utility::EngineClock::getFrameCount() const
{
    return frameCount;
}

///
/// Returns Physics Delta in Seconds
///
float Prism::Utility::EngineClock::getPhysicsDelta() const
{
    const float deltaSeconds = std::chrono::duration_cast<std::chrono::duration<float>>(physicsTime).count();
    return deltaSeconds;
}

///
/// Returns Physics Delta in Seconds
///
float Prism::Utility::EngineClock::getPhysicsDeltaMilliseconds() const
{
    return static_cast<float>(physicsTime.count()) / 1000000.0f;
}

///
/// Returns Update Delta in Seconds
///
float Prism::Utility::EngineClock::getUpdateDelta() const
{
    const float deltaSeconds = std::chrono::duration_cast<std::chrono::duration<float>>(updateTime).count();
    return deltaSeconds;
}

///
/// Returns Update Delta in Seconds
///
float Prism::Utility::EngineClock::getUpdateDeltaMilliseconds() const
{
    return static_cast<float>(updateTime.count()) / 1000000.0f;
}

///
/// Returns Render Delta in Seconds
///
float Prism::Utility::EngineClock::getRenderDelta() const
{
    const float deltaSeconds = std::chrono::duration_cast<std::chrono::duration<float>>(renderTime).count();
    return deltaSeconds;
}

///
/// Returns Render Delta in Seconds
///
float Prism::Utility::EngineClock::getRenderDeltaMilliseconds() const
{
    return static_cast<float>(renderTime.count()) / 1000000.0f;
}


void Prism::Utility::EngineClock::reset()
{
    frameCount = 0;

    startPhysicsTime = std::chrono::high_resolution_clock::now();
    startUpdateTime = std::chrono::high_resolution_clock::now();
    startRenderTime = std::chrono::high_resolution_clock::now();

    endPhysicsTime = startPhysicsTime;
    endUpdateTime = startUpdateTime;
    endRenderTime = startRenderTime;

    physicsTimerStarted = false;
    updateTimerStarted = false;
    renderTimerStarted = false;

    physicsTime = std::chrono::duration<long long, std::ratio<1, 1000000000>>::zero();
    updateTime = std::chrono::duration<long long, std::ratio<1, 1000000000>>::zero();
    renderTime = std::chrono::duration<long long, std::ratio<1, 1000000000>>::zero();
}
