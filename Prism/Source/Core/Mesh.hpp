#pragma once
#include "../Assets/StaticMeshAsset.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Core
{
    class Mesh
    {
    public:
        explicit Mesh(const RawPtr<Assets::MeshAsset> meshAsset, const uint64_t meshId) :
            meshAsset(std::move(meshAsset)), meshId(meshId)
        {
        }

        virtual ~Mesh() = default;

        [[nodiscard]] RawPtr<Assets::MeshAsset> getMeshAsset() const
        {
            return meshAsset;
        }

        [[nodiscard]] uint64_t getMeshId() const
        {
            return meshId;
        }

    protected:
        RawPtr<Assets::MeshAsset> meshAsset;

    private:
        uint64_t meshId;
    };
}
