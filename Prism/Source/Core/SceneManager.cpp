#include "SceneManager.hpp"

#include <stdexcept>

void Prism::Core::SceneManager::initialize()
{
}

void Prism::Core::SceneManager::initializeDeferred()
{
    activeScene->initInternal();
}

void Prism::Core::SceneManager::deInitialize()
{
    unloadScene();
}

void Prism::Core::SceneManager::loadScene(std::unique_ptr<Scene>&& scene)
{
    if (!scene)
    {
        throw std::runtime_error("SceneManager::loadScene: scene is nullptr");
    }
    unloadScene();
    activeScene = std::move(scene);
    activeScene->initInternal();
    activeScene->beginPlayInternal();
}

void Prism::Core::SceneManager::tick(const float deltaTime)
{
    activeScene->tickInternal(deltaTime);
    processPendingSpawnActors();
    processPendingKillActors();
}

RawPtr<Prism::Core::Actor> Prism::Core::SceneManager::registerActorInternal(std::unique_ptr<Actor>&& actor)
{
    const auto& emplacedActor = pendingSpawnActors.emplace_back(std::move(actor));
    return RawPtr<Prism::Core::Actor>(emplacedActor.get());
}

void Prism::Core::SceneManager::unregisterActor(RawPtr<Actor> actor)
{
    actor->setPendingKillInternal();
    pendingKillActors.emplace_back(actor);
}

void Prism::Core::SceneManager::unloadScene()
{
    pendingKillActors.clear();
    pendingSpawnActors.clear();
    if (activeScene)
    {
        activeScene->shutdownInternal();
        activeScene = nullptr;
    }
}

void Prism::Core::SceneManager::processPendingSpawnActors()
{
    for (auto& actor : pendingSpawnActors)
    {
        activeScene->registerActor(std::move(actor));
    }
    pendingSpawnActors.clear();
}

void Prism::Core::SceneManager::processPendingKillActors()
{
    for (const auto actor : pendingKillActors)
    {
        activeScene->unregisterActor(actor);
    }
    pendingKillActors.clear();
}
