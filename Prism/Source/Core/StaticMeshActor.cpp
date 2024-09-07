#include "StaticMeshActor.hpp"
#include "../Assets/AssetManager.hpp"

Prism::Core::StaticMeshActor::StaticMeshActor()
{
    setName("StaticMeshActor");
    staticMeshComponent = addComponent<StaticMeshComponent>();
}

void Prism::Core::StaticMeshActor::init()
{
    Actor::init();
}


void Prism::Core::StaticMeshActor::tick(float deltaTime)
{
    Actor::tick(deltaTime);
}

void Prism::Core::StaticMeshActor::shutdown()
{
    Actor::shutdown();
}
