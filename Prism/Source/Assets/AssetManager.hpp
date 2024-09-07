#pragma once
#include <vector>
#include <map>

#include "Asset.hpp"
#include "IAssetFactory.hpp"
#include "../Utilities/IService.hpp"
#include "../Utilities/TypeMap.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Assets
{
    template <typename T>
    concept SubtypeOfAsset = std::is_base_of_v<Asset, T> && !std::is_abstract_v<T>;

    template <typename T>
    concept ImplementsIAssetFactory = std::is_base_of_v<IAssetFactory, T> && !std::is_abstract_v<T>;

    class AssetManager final : public Utility::IService
    {
    public:
        ~AssetManager() override = default;

        template <SubtypeOfAsset T>
        RawPtr<T> getAsset(const std::string& name)
        {
            auto typeResult = assets.find<T>();
            RawPtr<std::vector<std::unique_ptr<Asset>>> assetList;

            // Create asset vector if it doesn't exist, otherwise retrieve it
            if (typeResult == assets.end())
            {
                std::vector<std::unique_ptr<Asset>> assetsOfType;
                assets.put<T>(std::move(assetsOfType));
                assetList = &assets.find<T>()->second;
            }
            else
            {
                assetList = &typeResult->second;
            }

            // Search for asset of name
            auto result = std::find_if(assetList->begin(), assetList->end(), [&name](auto& a)
            {
                return a->getName() == name;
            });

            // If asset doesn't exist, load and cache it
            if (result == assetList->end())
            {
                auto asset = loadAsset<T>(name);
                if (!asset)
                {
                    return RawPtr<T>();
                }
                // Cache object pointer here as we move the unique one afterwards!
                auto objAssetPtr = RawPtr<T>(static_cast<T*>(asset.get()));
                assetList->emplace_back(std::move(asset));
                return objAssetPtr;
            }

            return RawPtr<T>(static_cast<T*>(result->get()));
        }

        template <SubtypeOfAsset T>
        std::vector<RawPtr<T>> getAssets()
        {
            auto assetIter = assets.find<T>();
            if (assetIter != assets.end())
            {
                std::vector<RawPtr<T>> results;
                results.reserve(assetIter->second.size());
                for (auto& asset : assetIter->second)
                {
                    results.emplace_back(RawPtr<T>(static_cast<T*>(asset.get())));
                }
                return results;
            }

            return std::vector<RawPtr<T>>();
        }

        template <ImplementsIAssetFactory T, SubtypeOfAsset U>
        void registerAssetFactory()
        {
            assetFactories.put<U>(std::make_unique<T>());
        }

        void initialize() override
        {
        }

        void initializeDeferred() override
        {
        }

        void deInitialize() override
        {
        }

        std::string getFullName() override
        {
            return "Prism::Assets::AssetManager";
        }

    private:
        TypeMap<std::vector<std::unique_ptr<Asset>>> assets;
        TypeMap<std::unique_ptr<IAssetFactory>> assetFactories;

        template <SubtypeOfAsset T>
        std::unique_ptr<T> loadAsset(const std::string& name)
        {
            auto asset = assetFactories.find<T>()->second->loadAsset(name);
            if (!asset)
            {
                return nullptr;
            }
            return std::unique_ptr<T>(static_cast<T*>(asset.release()));
        }
    };
}
