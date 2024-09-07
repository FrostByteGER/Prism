#pragma once
#include <any>
#include "Scene.hpp"
#include "../Utilities/IService.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Core
{
    class ISceneManager : public Utility::IService
    {
    public:
        ~ISceneManager() override = default;
        virtual void initialize() override = 0;
        virtual void initializeDeferred() override = 0;
        virtual void deInitialize() override = 0;
        virtual std::string getFullName() override = 0;
        virtual void loadScene(std::unique_ptr<Scene>&& scene) = 0;
        virtual void tick(float deltaTime) = 0;

        template <ExtendsActor T>
        RawPtr<T> registerActor(std::unique_ptr<T>&& actor)
        {
            auto registeredActor = registerActorInternal(std::move(actor));
            return RawPtr<T>(dynamic_cast<T*>(registeredActor.get()));
        }

        virtual void unregisterActor(RawPtr<Actor> actor) = 0;
        [[nodiscard]] virtual RawPtr<Scene> getActiveScene() const = 0;

    protected:
        virtual RawPtr<Actor> registerActorInternal(std::unique_ptr<Actor>&& actor) = 0;
    };
}
