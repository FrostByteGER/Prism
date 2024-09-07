#pragma once
#include "glm/glm.hpp"

namespace Prism::Rendering
{
    struct UniformBufferObject
    {
        alignas(16) glm::mat4x4 view;
        alignas(16) glm::mat4x4 projection;
    };
}
