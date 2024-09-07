#include "ImGuiImplVulkan.hpp"

#include "imgui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "../VulkanRenderer.hpp"
#include <GLFW/glfw3.h>

void Prism::Rendering::Vulkan::ImGuiImplVulkan::init(GLFWwindow* glfwWindow, const RawPtr<VulkanRenderer> renderer,
                                                       vk::UniqueInstance& instance,
                                                       const vk::PhysicalDevice physicalDevice,
                                                       const RawPtr<vk::UniqueDevice> logicalDevicePtr,
                                                       const vk::RenderPass renderPass,
                                                       const vk::Queue graphicsQueue)
{
    this->vulkanRenderer = renderer;
    this->logicalDevice = logicalDevicePtr;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);

    try
    {
        std::array<vk::DescriptorPoolSize, 11> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eSampler;
        poolSizes[0].descriptorCount = 1000;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = 1000;
        poolSizes[2].type = vk::DescriptorType::eSampledImage;
        poolSizes[2].descriptorCount = 1000;
        poolSizes[3].type = vk::DescriptorType::eStorageImage;
        poolSizes[3].descriptorCount = 1000;
        poolSizes[4].type = vk::DescriptorType::eUniformTexelBuffer;
        poolSizes[4].descriptorCount = 1000;
        poolSizes[5].type = vk::DescriptorType::eStorageTexelBuffer;
        poolSizes[5].descriptorCount = 1000;
        poolSizes[6].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[6].descriptorCount = 1000;
        poolSizes[7].type = vk::DescriptorType::eStorageBuffer;
        poolSizes[7].descriptorCount = 1000;
        poolSizes[8].type = vk::DescriptorType::eUniformBufferDynamic;
        poolSizes[8].descriptorCount = 1000;
        poolSizes[9].type = vk::DescriptorType::eStorageBufferDynamic;
        poolSizes[9].descriptorCount = 1000;
        poolSizes[10].type = vk::DescriptorType::eInputAttachment;
        poolSizes[10].descriptorCount = 1000;

        vk::DescriptorPoolCreateInfo poolInfo = {};
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1000 * 11;
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

        descriptorPool = logicalDevice->get().createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError& e)
    {
        throw std::runtime_error("Failed to create ImGui descriptor pool: " + std::string(e.what()));
    }

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = instance.get();
    initInfo.PhysicalDevice = physicalDevice;
    initInfo.Device = logicalDevicePtr.get()->get();
    initInfo.RenderPass = renderPass;
    initInfo.QueueFamily = vulkanRenderer->findQueueFamilies(physicalDevice).graphicsFamily.value();
    initInfo.Queue = graphicsQueue;
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = descriptorPool;
    initInfo.Allocator = nullptr;
    initInfo.MinImageCount = vulkanRenderer->maxFramesInFlight;
    initInfo.ImageCount = vulkanRenderer->maxFramesInFlight;
    initInfo.CheckVkResultFn = [](const VkResult err)
    {
        if (err != VK_SUCCESS)
        {
            LOG_ERROR("Failed to initialize ImGui. Error Code: " + std::to_string(err));
        }
    };

    const bool loadedFunctions = ImGui_ImplVulkan_LoadFunctions([](const char* functionName, void* userData)
    {
        const vk::Instance* uniqueInstance = static_cast<vk::Instance*>(userData);
        return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(
            *uniqueInstance, functionName);
    }, &*instance);
    if (!loadedFunctions)
    {
        throw std::runtime_error("Failed to load Vulkan functions for ImGui");
    }
    ImGui_ImplVulkan_Init(&initInfo);

    ImGui_ImplVulkan_CreateFontsTexture();
    //ImGui_ImplVulkan_SetMinImageCount(vulkanRenderer->swapChainImages.size());
}

void Prism::Rendering::Vulkan::ImGuiImplVulkan::newFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    for (const auto& component : guiComponents)
    {
        component->renderUi();
    }
}

void Prism::Rendering::Vulkan::ImGuiImplVulkan::render(const vk::CommandBuffer& commandBuffer)
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void Prism::Rendering::Vulkan::ImGuiImplVulkan::shutdown() const
{
    for (const auto& component : guiComponents)
    {
        component->shutdown();
    }
    ImGui_ImplVulkan_Shutdown();
    logicalDevice->get().destroyDescriptorPool(descriptorPool);
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
