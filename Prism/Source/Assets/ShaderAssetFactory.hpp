#pragma once
#include "IAssetFactory.hpp"
#include "../Utilities/Logging/Log.hpp"
#include "ShaderAsset.hpp"

namespace Prism::Assets
{
    class ShaderAssetFactory : public IAssetFactory
    {
    public:
        std::unique_ptr<Asset> loadAsset(const std::string& fileName) override;
    };
}
