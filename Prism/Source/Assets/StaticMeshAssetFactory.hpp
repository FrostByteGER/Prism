#pragma once
#include "IAssetFactory.hpp"

namespace Prism::Assets
{
    class StaticMeshAssetFactory : public IAssetFactory
    {
    public:
        std::unique_ptr<Asset> loadAsset(const std::string& fileName) override;
    };
}
