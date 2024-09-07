#pragma once
#include "glm/glm.hpp"

namespace Prism::Rendering
{
    struct PushConstantObject
    {
        alignas(16) glm::mat4x4 model;
        alignas(16) glm::vec3 color;
        alignas(16) glm::vec3 lightPos;
    };
}
