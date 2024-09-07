#pragma once
#include "vulkan/vulkan.hpp"

namespace Prism::Rendering::Vulkan
{
    class VulkanBuffer
    {
    public:
        VulkanBuffer(const vk::Buffer vulkanBuffer, const vk::DeviceMemory vulkanBufferMemory) :
            buffer(vulkanBuffer),
            bufferMemory(vulkanBufferMemory)
        {
        }

        virtual ~VulkanBuffer() = default;

        [[nodiscard]] vk::Buffer getBuffer() const
        {
            return buffer;
        }

        [[nodiscard]] vk::DeviceMemory getBufferMemory() const
        {
            return bufferMemory;
        }

    private:
        vk::Buffer buffer;
        vk::DeviceMemory bufferMemory;
    };
}
