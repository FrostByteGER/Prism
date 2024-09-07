#pragma once
#include "Actor.hpp"
#include "ISceneManager.hpp"
#include "Scene.hpp"

namespace Prism::Core
{
    class SceneManager : public ISceneManager
    {
    public:
        ~SceneManager() override = default;
        void initialize() override;
        void initializeDeferred() override;
        void deInitialize() override;
        void loadScene(std::unique_ptr<Scene>&& scene) override;
        void tick(float deltaTime) override;

        RawPtr<Actor> registerActorInternal(std::unique_ptr<Actor>&& actor) override;
        void unregisterActor(RawPtr<Actor> actor) override;

        std::string getFullName() override
        {
            return "Prism::Core::SceneManager";
        }

        [[nodiscard]] RawPtr<Scene> getActiveScene() const override
        {
            return activeScene;
        }

    private:
        // Empty default scene
        std::unique_ptr<Scene> activeScene = std::make_unique<Scene>();
        std::vector<std::unique_ptr<Actor>> pendingSpawnActors;
        std::vector<RawPtr<Actor>> pendingKillActors;

        void unloadScene();
        void processPendingSpawnActors();
        void processPendingKillActors();
    };
}
