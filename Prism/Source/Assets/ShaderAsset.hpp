#pragma once
#include <vector>
#include "Asset.hpp"

namespace Prism::Assets
{
    class ShaderAsset : public Asset
    {
    public:
        explicit ShaderAsset(const std::string& name, const std::vector<char>& shader) : Asset(name), shader(shader)
        {
        }

        [[nodiscard]] std::vector<char> getShader() const
        {
            return shader;
        }

    private:
        std::vector<char> shader;
    };
}
