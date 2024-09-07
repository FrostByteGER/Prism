#pragma once
#include "Actor.hpp"
#include "ITickable.hpp"

namespace Prism::Core
{
    class Scene : public ITickable
    {
    public:
        void beginPlayInternal();
        void initInternal();
        void tick(float deltaTime) override;
        void tickInternal(float deltaTime);

        void shutdownInternal();


        //TODO: Move this out of scene, SceneManager shall handle everything
        template <ExtendsActor T>
        RawPtr<T> registerActor(std::unique_ptr<T>&& actor)
        {
            const auto& emplacedActor = actors.emplace_back(std::move(actor));
            if (initialized)
            {
                emplacedActor->initInternal();
            }
            deferredActors.emplace_back(RawPtr<Actor>(emplacedActor.get()));
            return RawPtr<T>(dynamic_cast<T*>(emplacedActor.get()));
        }

        void unregisterActor(RawPtr<Actor> actor);

        [[nodiscard]] std::vector<RawPtr<Actor>> getActors() const;

        template <ExtendsActor T>
        RawPtr<T> getActor() const
        {
            for (const auto& actor : actors)
            {
                auto castedActor = dynamic_cast<T*>(actor.get());
                if (castedActor)
                {
                    return RawPtr<T>(castedActor);
                }
            }
            return nullptr;
        }

        template <ExtendsActor T>
        RawPtr<T> getFirstActorWithName(const std::string& name) const
        {
            for (const auto& actor : actors)
            {
                if (actor->getName() != name)
                {
                    continue;
                }
                auto castedActor = dynamic_cast<T*>(actor.get());
                if (castedActor)
                {
                    return RawPtr<T>(castedActor);
                }
            }
            return nullptr;
        }

        template <ExtendsActor T>
        std::vector<RawPtr<T>> getActors() const
        {
            std::vector<RawPtr<T>> foundActors;
            for (const auto& actor : actors)
            {
                auto castedActor = dynamic_cast<T*>(actor.get());
                if (castedActor)
                {
                    foundActors.emplace_back(RawPtr<T>(castedActor));
                }
            }
            return foundActors;
        }

        template <ExtendsActor T>
        std::vector<RawPtr<T>> getActorsWithName(const std::string& name) const
        {
            std::vector<RawPtr<T>> foundActors;
            for (const auto& actor : actors)
            {
                if (actor->getName() != name)
                {
                    continue;
                }
                auto castedActor = dynamic_cast<T*>(actor.get());
                if (castedActor)
                {
                    foundActors.emplace_back(RawPtr<T>(castedActor));
                }
            }
            return foundActors;
        }

        template <ExtendsActorComponent T>
        std::vector<RawPtr<T>> getComponents() const
        {
            std::vector<RawPtr<T>> components;
            for (const auto& actor : actors)
            {
                for (auto component : actor->getComponents())
                {
                    auto castedComponent = dynamic_cast<T*>(component.get());
                    if (castedComponent)
                    {
                        components.emplace_back(RawPtr<T>(castedComponent));
                    }
                }
            }
            return components;
        }

    protected:
        virtual void init();
        virtual void beginPlay();
        virtual void shutdown();
        virtual void initGuiComponents();

    private:
        std::vector<std::unique_ptr<Actor>> actors;
        std::vector<RawPtr<Actor>> deferredActors;
        bool initialized = false;
    };
}
