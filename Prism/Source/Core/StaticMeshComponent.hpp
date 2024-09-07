#pragma once

#include "ActorComponent.hpp"
#include "../Core/StaticMesh.hpp"
#include "../Assets/StaticMeshAsset.hpp"

namespace Prism::Core
{
    class StaticMeshComponent : public ActorComponent
    {
    public:
        explicit StaticMeshComponent(const RawPtr<Actor>& parent)
            : ActorComponent(parent)
        {
            setName("StaticMeshComponent");
        }

        ~StaticMeshComponent() override;

        void init() override;
        void initDeferred() override;
        void shutdown() override;

        [[nodiscard]] bool isVisible() const
        {
            return visible;
        }

        void setVisible(const bool visible)
        {
            this->visible = visible;
        }

        [[nodiscard]] bool isCollidable() const
        {
            return collidable;
        }

        void setCollidable(const bool collidable)
        {
            this->collidable = collidable;
        }

        [[nodiscard]] RawPtr<Assets::StaticMeshAsset> getStaticMeshAsset() const
        {
            return staticMeshAsset;
        }

        void setStaticMeshAsset(const RawPtr<Assets::StaticMeshAsset> meshAsset)
        {
            this->staticMeshAsset = meshAsset;
        }

        [[nodiscard]] RawPtr<StaticMesh> getStaticMesh() const
        {
            return staticMesh;
        }

        [[nodiscard]] glm::vec3 getMeshColor() const
        {
            return meshColor;
        }

        void setMeshColor(const glm::vec3& meshColor)
        {
            this->meshColor = meshColor;
        }

    private:
        bool visible = true;
        bool collidable = true;
        RawPtr<Assets::StaticMeshAsset> staticMeshAsset;
        std::unique_ptr<StaticMesh> staticMesh;
        glm::vec3 meshColor;
    };
}
