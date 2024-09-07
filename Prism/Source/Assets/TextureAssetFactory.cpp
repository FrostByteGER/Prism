#include "TextureAssetFactory.hpp"
#include "TextureAsset.hpp"
#include "../Utilities/Logging/Log.hpp"
// This define and the following stb_image include needs to come last, otherwise we get multiple function bodies!
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

std::unique_ptr<Prism::Assets::Asset> Prism::Assets::TextureAssetFactory::loadAsset(const std::string& fileName)
{
    int texWidth;
    int texHeight;
    int texChannels;
    auto texPtr = deletable_stbi_uc_ptr(
        stbi_load(fileName.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha), stbi_image_free);
    if (!texPtr)
    {
        LOG_ERROR("Failed to open file: {}, error message: {}", fileName, std::string(stbi_failure_reason()));
        return nullptr;
    }
    return std::make_unique<TextureAsset>(fileName, std::move(texPtr), texWidth, texHeight, texChannels);
}
