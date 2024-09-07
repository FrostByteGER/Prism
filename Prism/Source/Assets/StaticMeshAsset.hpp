#pragma once

#include "MeshAsset.hpp"

namespace Prism::Assets
{
    class StaticMeshAsset : public MeshAsset
    {
    public:
        StaticMeshAsset(const std::string& name, std::vector<Rendering::Vertex> vertices, std::vector<uint32_t> indices)
            : MeshAsset(name, std::move(vertices), std::move(indices))
        {
        }
    };
}
