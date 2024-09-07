#pragma once
#include "Actor.hpp"
#include "StaticMeshComponent.hpp"

namespace Prism::Core
{
    class StaticMeshActor : public Actor
    {
    public:
        StaticMeshActor();
        ~StaticMeshActor() override = default;
        void tick(float deltaTime) override;

    protected:
        void init() override;
        void shutdown() override;

    private:
        RawPtr<StaticMeshComponent> staticMeshComponent;
    };
}
