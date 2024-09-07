#pragma once

#include <memory>
#include "Asset.hpp"

namespace Prism::Assets
{
    class IAssetFactory
    {
    public:
        virtual ~IAssetFactory() = default;
        virtual std::unique_ptr<Asset> loadAsset(const std::string& fileName) = 0;

    private:
    };
}
