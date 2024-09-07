#pragma once
#include "ActorComponent.hpp"

namespace Prism::Core
{
    class CameraComponent : public ActorComponent
    {
    public:
        explicit CameraComponent(const RawPtr<Actor>& parent) : ActorComponent(parent)
        {
            setName("CameraComponent");
        }

        void init() override;
        void shutdown() override;

        float fov = 45.0f;
        float zNear = 0.1f;
        float zFar = 100000.0f;
    };
}
