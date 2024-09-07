#pragma once
#include <string>
#include <glm/vec3.hpp>

#include "ITickable.hpp"
#include "Transform.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Core
{
    class Actor;

    class ActorComponent : public ITickable
    {
    public:
        explicit ActorComponent(const RawPtr<Actor>& parent) : parent(parent)
        {
        }

        ~ActorComponent() override = default;

        virtual void init();
        virtual void initDeferred();
        virtual void beginPlay();
        void tick(float deltaTime) override;
        virtual void shutdown();
        void translateComponent(const glm::vec3& translation);
        void rotateComponent(const glm::vec3& rotationDegrees);
        void scaleComponentUniform(float scaleFactor);
        void scaleComponent(const glm::vec3& scaleFactor);

        [[nodiscard]] std::string getName() const
        {
            return name;
        }

        void setName(const std::string& newName)
        {
            this->name = newName;
        }

        [[nodiscard]] glm::vec3 getComponentPosition() const
        {
            return transform.getTranslation();
        }

        void setComponentPosition(const glm::vec3& newPosition)
        {
            transform.setTranslation(newPosition);
        }

        [[nodiscard]] glm::vec3 getComponentRotation() const
        {
            return transform.getRotation();
        }

        void setComponentRotation(const glm::vec3& newRotationDegrees)
        {
            transform.setRotation(newRotationDegrees);
        }

        [[nodiscard]] glm::vec3 getComponentScale() const
        {
            return transform.getScale();
        }

        void setComponentScale(const glm::vec3& newScale)
        {
            transform.setScale(newScale);
        }

        [[nodiscard]] Transform getLocalTransform() const;

        [[nodiscard]] Transform getAbsoluteTransform();

    private:
        std::string name = "ActorComponent";
        Transform transform;
        RawPtr<Actor> parent;
    };
}
