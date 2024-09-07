#include "DebugLineActor.hpp"

void Prism::Core::DebugLineActor::init()
{
    StaticMeshActor::init();
}

void Prism::Core::DebugLineActor::initDeferred()
{
    StaticMeshActor::initDeferred();
    deathTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() + lifetimeMilliseconds;
}

void Prism::Core::DebugLineActor::shutdown()
{
    StaticMeshActor::shutdown();
}

void Prism::Core::DebugLineActor::tick(float deltaTime)
{
    StaticMeshActor::tick(deltaTime);
}
