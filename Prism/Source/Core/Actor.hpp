#pragma once
#include <memory>
#include <string>
#include <vector>

#include "ActorComponent.hpp"
#include "ITickable.hpp"
#include "Transform.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Core
{
    template <typename T>
    concept ExtendsActor = std::is_base_of_v<Actor, T>;

    template <typename T>
    concept ExtendsActorComponent = std::is_base_of_v<ActorComponent, T>;

    class Actor : public ITickable
    {
    public:
        Actor() = default;
        virtual ~Actor() override = default;
        void beginPlayInternal();
        void initInternal();
        void initDeferredInternal();
        virtual void tick(float deltaTime) override;
        void tickInternal(float deltaTime);
        void shutdownInternal();
        void translateActor(const glm::vec3& translation);
        void rotateActor(const glm::vec3& rotationDegrees);
        void scaleActorUniform(float scaleFactor);
        void scaleActor(const glm::vec3& scaleFactor);

        [[nodiscard]] std::string getName() const
        {
            return name;
        }

        void setName(const std::string& newName)
        {
            this->name = newName;
        }

        [[nodiscard]] std::vector<RawPtr<ActorComponent>> getComponents() const
        {
            std::vector<RawPtr<ActorComponent>> outComponents;
            std::ranges::transform(components, std::back_inserter(outComponents),
                                   [](const auto& component) { return component.get(); });
            return outComponents;
        }

        template <ExtendsActorComponent T>
        [[nodiscard]] RawPtr<T> getFirstComponentOfType() const
        {
            for (const auto& component : components)
            {
                auto castedComponent = dynamic_cast<T*>(component.get());
                if (castedComponent)
                {
                    return RawPtr<T>(castedComponent);
                }
            }
            return nullptr;
        }

        [[nodiscard]] glm::vec3 getActorPosition() const
        {
            return transform.getTranslation();
        }

        void setActorPosition(const glm::vec3& newPosition)
        {
            transform.setTranslation(newPosition);
        }

        [[nodiscard]] glm::vec3 getActorRotation() const
        {
            return transform.getRotation();
        }

        void setActorRotation(const glm::vec3& newRotationDegrees)
        {
            transform.setRotation(newRotationDegrees);
        }

        [[nodiscard]] glm::vec3 getActorScale() const
        {
            return transform.getScale();
        }

        void setActorScale(const glm::vec3& newScale)
        {
            transform.setScale(newScale);
        }

        [[nodiscard]] Transform getTransform() const
        {
            return transform;
        }

        template <ExtendsActorComponent T>
        RawPtr<T> addComponent()
        {
            auto component = std::make_unique<T>(this);
            if (initialized)
            {
                component->init();
            }
            auto returnComponent = RawPtr<T>(component.get());
            components.emplace_back(std::move(component));
            return returnComponent;
        }

        [[nodiscard]] bool isValid() const
        {
            return !pendingKill;
        }

        void setPendingKillInternal()
        {
            pendingKill = true;
        }

    protected:
        Transform transform;

        virtual void init();
        virtual void beginPlay();
        virtual void initDeferred();
        virtual void shutdown();

    private:
        std::string name = "Actor";
        bool initialized = false;
        bool initializedDeferred = false;
        bool pendingKill = false;
        std::vector<std::unique_ptr<ActorComponent>> components;
    };
}
