#pragma once
#include <optional>

#include "../ICameraManager.hpp"
#include "../../Core/ISceneManager.hpp"
#include "vulkan/vulkan.hpp"
#include "../IRenderer.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanMesh.hpp"
#include "ImGui/ImGuiImplVulkan.hpp"
#include "../GraphicsDebugBridge.hpp"

struct GLFWwindow;

namespace Prism::Rendering::Vulkan
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;

        [[nodiscard]] bool isComplete() const
        {
            return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    class VulkanRenderer : public IRenderer
    {
    public:
        inline constexpr static uint32_t maxFramesInFlight = 2;
        vk::Format swapChainImageFormat = vk::Format::eUndefined;
        std::vector<vk::Image> swapChainImages;

        explicit VulkanRenderer(GLFWwindow* newGlfwWindow);
        void init() override;
        void render() override;
        void shutdown() override;
        RawPtr<AbstractImmediateModeGui> getImmediateModeGui() override;
        void onFrameBufferResized(int width, int height) override;
        void registerNewStaticMesh(RawPtr<Core::StaticMesh> staticMesh) override;
        void unregisterStaticMesh(uint64_t staticMeshId) override;
        vk::CommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(vk::CommandBuffer commandBuffer);
        [[nodiscard]] QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) const;
        void createBuffer(vk::DeviceSize size, const vk::BufferUsageFlags& usage,
                          const vk::MemoryPropertyFlags& properties, vk::Buffer& buffer,
                          vk::DeviceMemory& bufferMemory);

    private:
        GLFWwindow* glfwWindow;

        RawPtr<GraphicsDebugBridge> graphicsDebugBridge;

        RawPtr<ICameraManager> cameraManager;
        RawPtr<Core::ISceneManager> sceneManager;
        ImGuiImplVulkan imGuiImpl;

        vk::UniqueInstance instance;
        vk::UniqueDebugUtilsMessengerEXT debugMessenger;
        vk::PhysicalDevice physicalDevice;
        vk::UniqueDevice logicalDevice;
        vk::SurfaceKHR surface;

        vk::Queue graphicsQueue;
        vk::Queue presentQueue;

        vk::SwapchainKHR swapChain;


        vk::Extent2D swapChainExtent;
        std::vector<vk::ImageView> swapChainImageViews;
        std::vector<vk::Framebuffer> swapChainFramebuffers;

        vk::RenderPass renderPass;
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::PipelineLayout pipelineLayout;
        vk::Pipeline graphicsPipeline;

        vk::CommandPool globalCommandPool;

        vk::Image textureImage;
        vk::DeviceMemory textureImageMemory;
        vk::ImageView textureImageView;
        vk::Sampler textureSampler;

        std::vector<std::shared_ptr<VulkanMesh>> meshes;
        std::vector<uint64_t> pendingMeshIdsToBeDeleted;

        vk::Image depthImage;
        vk::DeviceMemory depthImageMemory;
        vk::ImageView depthImageView;

        std::vector<vk::Buffer> uniformBuffers;
        std::vector<vk::DeviceMemory> uniformBuffersMemory;

        vk::DescriptorPool descriptorPool;
        std::vector<vk::DescriptorSet> descriptorSets;

        std::vector<vk::CommandPool, std::allocator<vk::CommandPool>> commandPools;
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;

        std::vector<vk::Semaphore> imageAvailableSemaphores;
        std::vector<vk::Semaphore> renderFinishedSemaphores;
        std::vector<vk::Fence> inFlightFences;
        size_t currentFrame = 0;

        bool framebufferResized = false;

        std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        // Be aware that "VK_KHR_SWAPCHAIN_EXTENSION_NAME" is a macro string!
        std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
            VK_KHR_RAY_QUERY_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
        };

#ifdef Prism_DEBUG
        bool enableValidationLayers = true;
#else
        bool enableValidationLayers = false;
#endif

        void createInstance();
        [[nodiscard]] std::vector<const char*> getExtensions() const;
        [[nodiscard]] bool checkValidationLayerSupport() const;
        void setupDebugCallback();
        void createSurface();
        void pickPhysicalDevice();
        [[nodiscard]] uint32_t calculateDeviceSuitability(const vk::PhysicalDevice& device) const;

        void createLogicalDevice();
        [[nodiscard]] bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device) const;
        [[nodiscard]] SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const;
        void createSwapChain();
        std::unique_ptr<VulkanBuffer> createVertexBuffer(const std::vector<Vertex>& vertices);
        std::unique_ptr<VulkanBuffer> createIndexBuffer(const std::vector<uint32_t>& indices);
        void recreateSwapChain();
        void cleanupSwapChain();
        [[nodiscard]] vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
        [[nodiscard]] vk::PresentModeKHR chooseSwapPresentMode(
            const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
        [[nodiscard]] vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPools();
        vk::CommandPool createCommandPool();
        void createDepthResources();
        vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
                                       const vk::FormatFeatureFlags& features) const;
        vk::Format findDepthFormat() const;
        bool hasStencilComponent(vk::Format format);
        void createTextureImage();
        void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
                         vk::ImageUsageFlags usage,
                         vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
        void createTextureImageView();
        vk::ImageView createImageView(const vk::Image& image, const vk::Format& format,
                                      const vk::ImageAspectFlags& aspectFlags);
        void createTextureSampler();
        void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout,
                                   vk::ImageLayout newLayout);
        void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
        void createUniformBuffers();
        void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize size,
                        vk::CommandBuffer commandBuffer = {nullptr});
        [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, const vk::MemoryPropertyFlags& properties) const;
        void createCommandBuffers();
        void createSyncObjects();
        void createCommandBuffer(const vk::CommandBuffer& commandBuffer, uint32_t currentImage);
        void updateCommandBuffer(uint32_t currentImage, uint32_t imageIndex);
        void updateUniformBuffer(uint32_t currentImage);
        void createDescriptorPool();
        void createDescriptorSets();
        vk::UniqueShaderModule createShaderModule(const std::vector<char>& shaderCode);
        void createDescriptorSetLayout();
        void cleanupDeadMeshes();
    };
}
