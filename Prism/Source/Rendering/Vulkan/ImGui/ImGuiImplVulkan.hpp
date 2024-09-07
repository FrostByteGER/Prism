#pragma once

#include "vulkan/vulkan.hpp"
#include "AbstractImmediateModeGui.hpp"
#include "../../../Utilities/Globals.hpp"

struct GLFWwindow;

namespace Prism::Rendering::Vulkan
{
    class VulkanRenderer;

    class ImGuiImplVulkan : public AbstractImmediateModeGui
    {
    public:
        explicit ImGuiImplVulkan() = default;
        void init(GLFWwindow* glfwWindow, RawPtr<VulkanRenderer> renderer, vk::UniqueInstance& instance,
                  vk::PhysicalDevice physicalDevice, RawPtr<vk::UniqueDevice> logicalDevicePtr,
                  vk::RenderPass renderPass,
                  vk::Queue graphicsQueue);
        void newFrame();
        void render(const vk::CommandBuffer& commandBuffer);
        void shutdown() const;

    private:
        vk::DescriptorPool descriptorPool;
        RawPtr<vk::UniqueDevice> logicalDevice;
        RawPtr<VulkanRenderer> vulkanRenderer;
    };
}
