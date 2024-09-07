#include "Scene.hpp"

void Prism::Core::Scene::init()
{
}

void Prism::Core::Scene::initGuiComponents()
{
}

void Prism::Core::Scene::initInternal()
{
    init();
    //TODO: Before or after actor->initInternal()?
    initGuiComponents();
    for (const auto& actor : actors)
    {
        actor->initInternal();
    }
    initialized = true;
}

void Prism::Core::Scene::beginPlay()
{
}

void Prism::Core::Scene::beginPlayInternal()
{
    beginPlay();
    for (const auto& actor : actors)
    {
        actor->beginPlayInternal();
    }
}

void Prism::Core::Scene::tick(const float deltaTime)
{
}

void Prism::Core::Scene::tickInternal(const float deltaTime)
{
    for (const auto& deferredActor : deferredActors)
    {
        deferredActor->initDeferredInternal();
    }
    deferredActors.clear();

    for (const auto& actor : actors)
    {
        actor->tickInternal(deltaTime);
    }
    tick(deltaTime);
}

void Prism::Core::Scene::shutdown()
{
}


void Prism::Core::Scene::shutdownInternal()
{
    for (const auto& toDelete : actors)
    {
        toDelete->shutdownInternal();
    }
    actors.clear();
    assert(actors.empty());
    shutdown();
}

void Prism::Core::Scene::unregisterActor(RawPtr<Actor> actor)
{
    actor->shutdownInternal();
    std::erase_if(deferredActors, [actor](const auto& a)
    {
        return a.get() == actor.get();
    });
    std::erase_if(actors, [actor](const auto& a)
    {
        return a.get() == actor.get();
    });
}

std::vector<RawPtr<Prism::Core::Actor>> Prism::Core::Scene::getActors() const
{
    std::vector<RawPtr<Actor>> allActors;
    allActors.reserve(actors.size());
    for (const auto& actor : actors)
    {
        allActors.emplace_back(actor.get());
    }

    return allActors;
}
