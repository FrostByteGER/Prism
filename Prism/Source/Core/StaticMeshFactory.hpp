#pragma once
#include "IMeshFactory.hpp"
#include "StaticMesh.hpp"

namespace Prism::Core
{
    class StaticMeshFactory : public IMeshFactory<StaticMesh, Assets::StaticMeshAsset>
    {
    public:
        StaticMeshFactory() = default;
        ~StaticMeshFactory() override = default;

        [[nodiscard]] std::unique_ptr<StaticMesh> createMesh(RawPtr<Assets::StaticMeshAsset> staticMeshAsset) override;

    private:
        uint64_t meshIdCounter = 0;
    };
}
