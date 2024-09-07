#pragma once

#include <vector>
#include "../Rendering/Vertex.hpp"

#include "Asset.hpp"

namespace Prism::Assets
{
    class MeshAsset : public Asset
    {
    public:
        explicit MeshAsset(const std::string& name, std::vector<Rendering::Vertex> vertices,
                           std::vector<uint32_t> indices)
            : Asset(name), vertices(std::move(vertices)), indices(std::move(indices))
        {
        }

        [[nodiscard]] std::vector<Rendering::Vertex>& getVertices()
        {
            return vertices;
        }

        [[nodiscard]] std::vector<uint32_t>& getIndices()
        {
            return indices;
        }

    private:
        std::vector<Rendering::Vertex> vertices;
        std::vector<uint32_t> indices;
    };
}
