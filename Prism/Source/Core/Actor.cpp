#include "Actor.hpp"
#include "ActorComponent.hpp"

void Prism::Core::Actor::init()
{
}

void Prism::Core::Actor::initDeferred()
{
}

void Prism::Core::Actor::beginPlay()
{
}

void Prism::Core::Actor::beginPlayInternal()
{
    beginPlay();
    for (const auto& component : components)
    {
        component->beginPlay();
    }
}

void Prism::Core::Actor::initInternal()
{
    for (const auto& component : components)
    {
        component->init();
    }
    init();
    initialized = true;
}

void Prism::Core::Actor::initDeferredInternal()
{
    for (const auto& component : components)
    {
        component->initDeferred();
    }
    initDeferred();
    initializedDeferred = true;
}

void Prism::Core::Actor::tick(const float deltaTime)
{
}

void Prism::Core::Actor::tickInternal(const float deltaTime)
{
    tick(deltaTime);
    for (const auto& component : components)
    {
        component->tick(deltaTime);
    }
}

void Prism::Core::Actor::shutdown()
{
}

void Prism::Core::Actor::shutdownInternal()
{
    for (const auto& component : components)
    {
        component->shutdown();
    }
    shutdown();
}

void Prism::Core::Actor::translateActor(const glm::vec3& translation)
{
    transform.translate(translation);
}

void Prism::Core::Actor::rotateActor(const glm::vec3& rotationDegrees)
{
    transform.rotate(rotationDegrees);
}

void Prism::Core::Actor::scaleActorUniform(const float scaleFactor)
{
    transform.scale(scaleFactor);
}

void Prism::Core::Actor::scaleActor(const glm::vec3& scaleFactor)
{
    transform.scale(scaleFactor);
}
