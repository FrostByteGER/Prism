#include "Actor.hpp"
#include "ActorComponent.hpp"

void Prism::Core::ActorComponent::init()
{
}

void Prism::Core::ActorComponent::initDeferred()
{
}

void Prism::Core::ActorComponent::beginPlay()
{
}

void Prism::Core::ActorComponent::tick(const float deltaTime)
{
}

void Prism::Core::ActorComponent::shutdown()
{
}

void Prism::Core::ActorComponent::translateComponent(const glm::vec3& translation)
{
    transform.translate(translation);
}

void Prism::Core::ActorComponent::rotateComponent(const glm::vec3& rotationDegrees)
{
    transform.rotate(rotationDegrees);
}

void Prism::Core::ActorComponent::scaleComponentUniform(const float scaleFactor)
{
    transform.scale(scaleFactor);
}

void Prism::Core::ActorComponent::scaleComponent(const glm::vec3& scaleFactor)
{
    transform.scale(scaleFactor);
}

[[nodiscard]] Prism::Core::Transform Prism::Core::ActorComponent::getLocalTransform() const
{
    return transform;
}

[[nodiscard]] Prism::Core::Transform Prism::Core::ActorComponent::getAbsoluteTransform()
{
    if (parent)
    {
        auto parentTransform = parent->getTransform();
        return transform.combineWithParent(parentTransform);
    }
    return transform;
}
