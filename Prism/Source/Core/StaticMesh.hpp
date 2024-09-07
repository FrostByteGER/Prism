#pragma once
#include "Mesh.hpp"

namespace Prism::Core
{
    class StaticMesh : public Mesh
    {
    public:
        explicit StaticMesh(const RawPtr<Assets::StaticMeshAsset> staticMeshAsset, const uint64_t meshId)
            : Mesh(staticMeshAsset, meshId)
        {
        }
    };
}
