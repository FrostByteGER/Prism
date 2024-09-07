#pragma once
#include <memory>
#include "Asset.hpp"
#include <stb_image.h>
#include "../Utilities/Globals.hpp"

namespace Prism::Assets
{
    using deletable_stbi_uc_ptr = std::unique_ptr<stbi_uc, void(*)(void*)>;

    class TextureAsset : public Asset
    {
    public:
        explicit TextureAsset(const std::string& name, deletable_stbi_uc_ptr&& texturePtr, const uint32_t width,
                              const uint32_t height, const uint32_t channels)
            : Asset(name), texture(std::move(texturePtr)), width(width), height(height), channels(channels)
        {
        }

        [[nodiscard]] RawPtr<stbi_uc> getTexture() const
        {
            return texture;
        }

        [[nodiscard]] uint32_t getWidth() const
        {
            return width;
        }

        [[nodiscard]] uint32_t getHeight() const
        {
            return height;
        }

        [[nodiscard]] uint32_t getChannels() const
        {
            return channels;
        }

        [[nodiscard]] constexpr uint32_t getBitsPerChannel()
        {
            return 8;
        }

    private:
        deletable_stbi_uc_ptr texture;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
    };
}
