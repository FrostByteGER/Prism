#include <map>
#include <set>
#include <chrono>
#include "../../Utilities/ServiceLocator.hpp"
#include "../../Assets/AssetManager.hpp"
#include "../../Assets/ShaderAsset.hpp"
#include "../../Assets/TextureAsset.hpp"
#include "../../Core/StaticMeshComponent.hpp"
#include "../../Utilities/Logging/Log.hpp"
#include "../Vertex.hpp"
#include "../UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "../PushConstantObject.hpp"
#include "VulkanBuffer.hpp"

// Include these two last to avoid windows macro redefinitions!
// VulkanRenderer.hpp BEFORE glfw3.h!!!
#include "VulkanRenderer.hpp"


VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <GLFW/glfw3.h>


namespace Prism::Rendering::Vulkan
{
    //TODO: Print messageType
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData)
    {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_DEBUG(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            LOG_CRITICAL(pCallbackData->pMessage);
            break;
        }

        return VK_FALSE;
    }

    VulkanRenderer::VulkanRenderer(GLFWwindow* newGlfwWindow)
    {
        this->glfwWindow = newGlfwWindow;
        this->cameraManager = Utility::ServiceLocator::getService<ICameraManager>();
        this->sceneManager = Utility::ServiceLocator::getService<Core::ISceneManager>();
        this->graphicsDebugBridge = Utility::ServiceLocator::getService<GraphicsDebugBridge>();
    }

    void VulkanRenderer::init()
    {
        createInstance();
        setupDebugCallback();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createCommandPools();
        createDepthResources();
        createFramebuffers();
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();

        imGuiImpl.init(glfwWindow, this, instance, physicalDevice, &logicalDevice, renderPass, graphicsQueue);

        createCommandBuffers();
        createSyncObjects();
    }

    void VulkanRenderer::render()
    {
        const auto waitForFencesResult = logicalDevice->waitForFences(1, &inFlightFences[currentFrame], VK_TRUE,
                                                                      std::numeric_limits<uint64_t>::max());
        if (waitForFencesResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to wait for fences: " + vk::to_string(waitForFencesResult));
        }

        for (const auto& meshIdToBeDeleted : pendingMeshIdsToBeDeleted)
        {
            std::erase_if(meshes, [meshIdToBeDeleted](const std::shared_ptr<VulkanMesh>& vulkanMesh)
            {
                if (!vulkanMesh->hasMeshIdAssociated(meshIdToBeDeleted))
                {
                    return false;
                }
                vulkanMesh->unassociateMeshId(meshIdToBeDeleted);
                return vulkanMesh->getMeshIds().empty();
            });
        }
        pendingMeshIdsToBeDeleted.clear();

        uint32_t imageIndex;
        try
        {
            const auto acquireNextImageKHRResult = logicalDevice->acquireNextImageKHR(
                swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], nullptr);
            imageIndex = acquireNextImageKHRResult.value;
        }
        catch (vk::OutOfDateKHRError&)
        {
            recreateSwapChain();
            return;
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to acquire next image from swapchain: " + std::string(e.what()));
        }

        updateCommandBuffer(static_cast<uint32_t>(currentFrame), imageIndex);
        updateUniformBuffer(static_cast<uint32_t>(currentFrame));


        vk::SubmitInfo submitInfo = {};

        const std::array waitSemaphores = {imageAvailableSemaphores[currentFrame]};
        constexpr std::array<vk::PipelineStageFlags, 1> waitStages = {
            vk::PipelineStageFlagBits::eColorAttachmentOutput
        };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        const std::array signalSemaphores = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores.data();

        const auto resetFencesResult = logicalDevice->resetFences(1, &inFlightFences[currentFrame]);
        if (resetFencesResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to reset fences: " + vk::to_string(resetFencesResult));
        }

        try
        {
            graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to submit draw commandbuffer: " + std::string(e.what()));
        }

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores.data();

        const std::array swapChains = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains.data();
        presentInfo.pImageIndices = &imageIndex;

        vk::Result presentKhrResult;
        try
        {
            presentKhrResult = presentQueue.presentKHR(presentInfo);
        }
        catch (vk::OutOfDateKHRError&)
        {
            presentKhrResult = vk::Result::eErrorOutOfDateKHR;
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to present new image to the swapchain: " + std::string(e.what()));
        }
        if (presentKhrResult == vk::Result::eSuboptimalKHR || framebufferResized)
        {
            LOG_DEBUG("Swapchain out of date/suboptimal/window resized - recreating!");
            framebufferResized = false;
            recreateSwapChain();
            return;
        }

        currentFrame = (currentFrame + 1) & maxFramesInFlight;
    }

    void VulkanRenderer::cleanupSwapChain()
    {
        logicalDevice->destroyImageView(depthImageView);
        logicalDevice->destroyImage(depthImage);
        logicalDevice->freeMemory(depthImageMemory);

        for (const auto& framebuffer : swapChainFramebuffers)
        {
            logicalDevice->destroyFramebuffer(framebuffer);
        }

        logicalDevice->destroyRenderPass(renderPass);
        logicalDevice->destroyPipeline(graphicsPipeline);
        logicalDevice->destroyPipelineLayout(pipelineLayout);

        for (const auto& imageView : swapChainImageViews)
        {
            logicalDevice->destroyImageView(imageView);
        }
        logicalDevice->destroySwapchainKHR(swapChain);
    }

    void VulkanRenderer::shutdown()
    {
        logicalDevice->waitIdle();

        imGuiImpl.shutdown();

        cleanupSwapChain();

        for (const auto& commandPool : commandPools)
        {
            logicalDevice->destroyCommandPool(commandPool);
        }


        logicalDevice->destroySampler(textureSampler);
        logicalDevice->destroyImageView(textureImageView);

        logicalDevice->destroyImage(textureImage);
        logicalDevice->freeMemory(textureImageMemory);

        // Destroys all meshes and their buffers
        meshes.clear();

        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            logicalDevice->destroyBuffer(uniformBuffers[i]);
            logicalDevice->freeMemory(uniformBuffersMemory[i]);
        }

        logicalDevice->destroyDescriptorPool(descriptorPool);
        logicalDevice->destroyDescriptorSetLayout(descriptorSetLayout);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            logicalDevice->destroySemaphore(renderFinishedSemaphores[i], nullptr);
            logicalDevice->destroySemaphore(imageAvailableSemaphores[i], nullptr);
            logicalDevice->destroyFence(inFlightFences[i], nullptr);
        }

        logicalDevice->destroyCommandPool(globalCommandPool);

        instance->destroySurfaceKHR(surface);
    }

    RawPtr<AbstractImmediateModeGui> VulkanRenderer::getImmediateModeGui()
    {
        return &imGuiImpl;
    }

    void VulkanRenderer::onFrameBufferResized(int width, int height)
    {
        framebufferResized = true;
    }

    void VulkanRenderer::recreateSwapChain()
    {
        int width = 0;
        int height = 0;
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(glfwWindow, &width, &height);
            glfwWaitEvents();
        }

        logicalDevice->waitIdle();

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createDepthResources();
        createFramebuffers();
        createCommandBuffers();
    }


    std::unique_ptr<VulkanBuffer> VulkanRenderer::createVertexBuffer(const std::vector<Vertex>& vertices)
    {
        const vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer, stagingBufferMemory);

        void* data = logicalDevice->mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, vertices.data(), bufferSize);
        logicalDevice->unmapMemory(stagingBufferMemory);

        vk::Buffer vertexBuffer;
        vk::DeviceMemory vertexBufferMemory;
        createBuffer(
            bufferSize,
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        logicalDevice->destroyBuffer(stagingBuffer);
        logicalDevice->freeMemory(stagingBufferMemory);

        return std::make_unique<VulkanBuffer>(vertexBuffer, vertexBufferMemory);
    }


    std::unique_ptr<VulkanBuffer> VulkanRenderer::createIndexBuffer(const std::vector<uint32_t>& indices)
    {
        const vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer, stagingBufferMemory);
        void* data = logicalDevice->mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, indices.data(), bufferSize);
        logicalDevice->unmapMemory(stagingBufferMemory);
        vk::Buffer indexBuffer;
        vk::DeviceMemory indexBufferMemory;
        createBuffer(
            bufferSize,
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
            vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);
        copyBuffer(stagingBuffer, indexBuffer, bufferSize);
        logicalDevice->destroyBuffer(stagingBuffer);
        logicalDevice->freeMemory(stagingBufferMemory);
        return std::make_unique<VulkanBuffer>(indexBuffer, indexBufferMemory);
    }

    void VulkanRenderer::registerNewStaticMesh(
        const RawPtr<Core::StaticMesh> staticMesh)
    {
        const auto meshIter = std::ranges::find_if(meshes, [staticMesh](const std::shared_ptr<VulkanMesh>& vulkanMesh)
        {
            return vulkanMesh->getMeshAssetName() == staticMesh->getMeshAsset()->getName();
        });
        if (meshIter != meshes.end())
        {
            if ((*meshIter)->associateMeshId(staticMesh->getMeshId()))
            {
                return;
            }
            throw std::runtime_error("Failed to associate mesh id with existing mesh! This should never happen!");
        }
        auto vertexBuffer = createVertexBuffer(staticMesh->getMeshAsset()->getVertices());
        auto indexBuffer = createIndexBuffer(staticMesh->getMeshAsset()->getIndices());
        auto vulkanMesh = std::make_shared<
            VulkanMesh>(&logicalDevice, staticMesh->getMeshAsset()->getName(),
                        std::vector{staticMesh->getMeshId()},
                        std::move(vertexBuffer), std::move(indexBuffer));
        meshes.emplace_back(vulkanMesh);
    }

    void VulkanRenderer::unregisterStaticMesh(const uint64_t staticMeshId)
    {
        pendingMeshIdsToBeDeleted.emplace_back(staticMeshId);
    }

    void VulkanRenderer::createInstance()
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();
        constexpr vk::ApplicationInfo appInfo("Prism",
                                              VK_MAKE_API_VERSION(0, 1, 0, 0),
                                              "Prism",
                                              VK_MAKE_API_VERSION(0, 1, 0, 0), VK_MAKE_API_VERSION(0, 1, 3, 0));

        const std::vector<const char*> extensions = getExtensions();

        uint32_t enabledValidationLayerCount = 0;
        std::vector<const char*> enabledValidationLayers;
        vk::ValidationFeaturesEXT validationFeatures;
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("Requested validation layers but none are available!");
        }
        else if (enableValidationLayers)
        {
            enabledValidationLayerCount = static_cast<uint32_t>(validationLayers.size());
            enabledValidationLayers = validationLayers;

            // Enable shader debug printf, off by default as it clashes with Profiling tools like Nvidia NSight Graphics
#define DEBUG_SHADERS
#ifdef DEBUG_SHADERS
            constexpr std::array enabledFeatures = {
                vk::ValidationFeatureEnableEXT::eDebugPrintf //, vk::ValidationFeatureEnableEXT::eBestPractices
            };
#else
            constexpr std::array<vk::ValidationFeatureEnableEXT, 0> enabledFeatures;
#endif
            validationFeatures.enabledValidationFeatureCount = static_cast<uint32_t>(enabledFeatures.size());
            validationFeatures.pEnabledValidationFeatures = enabledFeatures.data();
        }
        const vk::InstanceCreateInfo createInfo({}, &appInfo, enabledValidationLayerCount,
                                                enabledValidationLayers.data(),
                                                static_cast<uint32_t>(extensions.size()), extensions.data(),
                                                &validationFeatures);
        try
        {
            instance = vk::createInstanceUnique(createInfo);
            VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create instance: " + std::string(e.what()));
        }
    }

    std::vector<const char*> VulkanRenderer::getExtensions() const
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers)
        {
            extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool VulkanRenderer::checkValidationLayerSupport() const
    {
        const auto availableLayers = vk::enumerateInstanceLayerProperties();
        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for (const vk::LayerProperties& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void VulkanRenderer::setupDebugCallback()
    {
        if (!enableValidationLayers)
        {
            return;
        }

        const auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            debugCallback,
            nullptr
        );
        debugMessenger = instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr);
    }

    void VulkanRenderer::createSurface()
    {
        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(*instance, glfwWindow, nullptr, &rawSurface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan window surface!");
        }

        surface = rawSurface;
    }

    void VulkanRenderer::pickPhysicalDevice()
    {
        const auto devices = instance->enumeratePhysicalDevices();
        if (devices.empty())
        {
            throw std::runtime_error("No GPU with Vulkan support detected!");
        }
        std::multimap<uint32_t, vk::PhysicalDevice> candidates;
        for (const auto& device : devices)
        {
            const uint32_t score = calculateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }
        if (candidates.rbegin()->first > 0)
        {
            physicalDevice = candidates.rbegin()->second;
        }
        else
        {
            throw std::runtime_error("No suitable GPU found!");
        }
    }

    uint32_t VulkanRenderer::calculateDeviceSuitability(const vk::PhysicalDevice& device) const
    {
        uint32_t score = 0;
        const QueueFamilyIndices indices = findQueueFamilies(device);
        if (!indices.isComplete())
        {
            return 0;
        }

        const bool extensionsSupported = checkDeviceExtensionSupport(device);
        if (!extensionsSupported)
        {
            return 0;
        }

        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        const bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        if (!swapChainAdequate)
        {
            return 0;
        }

        const vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
        const vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

        // We require anisotropic filtering!
        if (!deviceFeatures.samplerAnisotropy)
        {
            return 0;
        }

        if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            score += 1000;
        }
        else if (deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
        {
            score += 500;
        }
        else if (deviceProperties.deviceType == vk::PhysicalDeviceType::eVirtualGpu)
        {
            score += 250;
        }
        else if (deviceProperties.deviceType == vk::PhysicalDeviceType::eCpu)
        {
            score += 10;
        }
        else if (deviceProperties.deviceType == vk::PhysicalDeviceType::eOther)
        {
            score += 0;
        }

        return score;
    }

    QueueFamilyIndices VulkanRenderer::findQueueFamilies(const vk::PhysicalDevice& device) const
    {
        QueueFamilyIndices indices;

        const auto queueFamilies = device.getQueueFamilyProperties();
        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
            }

            if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, surface))
            {
                indices.presentFamily = i;
            }

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
            {
                indices.computeFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }

            ++i;
        }

        return indices;
    }

    void VulkanRenderer::createLogicalDevice()
    {
        const QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        const std::set uniqueQueueFamilies = {
            indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value()
        };

        float queuePriority = 1.0f;

        queueCreateInfos.reserve(uniqueQueueFamilies.size());
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos.emplace_back(
                vk::DeviceQueueCreateFlags(),
                queueFamily,
                1,
                &queuePriority
            );
        }

        vk::PhysicalDeviceFeatures deviceFeatures;
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        auto createInfo = vk::DeviceCreateInfo(
            vk::DeviceCreateFlags(),
            static_cast<uint32_t>(queueCreateInfos.size()),
            queueCreateInfos.data()
        );
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        // Relevant for older versions of vulkan
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }

        try
        {
            logicalDevice = physicalDevice.createDeviceUnique(createInfo);
            VULKAN_HPP_DEFAULT_DISPATCHER.init(*logicalDevice);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create logical device from selected GPU: " + std::string(e.what()));
        }

        graphicsQueue = logicalDevice->getQueue(indices.graphicsFamily.value(), 0);
        presentQueue = logicalDevice->getQueue(indices.presentFamily.value(), 0);
    }

    bool VulkanRenderer::checkDeviceExtensionSupport(const vk::PhysicalDevice& device) const
    {
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const auto& extension : device.enumerateDeviceExtensionProperties())
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails VulkanRenderer::querySwapChainSupport(const vk::PhysicalDevice& device) const
    {
        SwapChainSupportDetails details;
        details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
        details.formats = device.getSurfaceFormatsKHR(surface);
        details.presentModes = device.getSurfacePresentModesKHR(surface);
        return details;
    }

    void VulkanRenderer::createSwapChain()
    {
        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        const vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        const vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        const vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo(
            vk::SwapchainCreateFlagsKHR(),
            surface,
            imageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment
        );

        const QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        const std::array queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
        }
        else
        {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

        try
        {
            swapChain = logicalDevice->createSwapchainKHR(createInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create swapchain: " + std::string(e.what()));
        }

        swapChainImages = logicalDevice->getSwapchainImagesKHR(swapChain);

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    vk::SurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR>& availableFormats) const
    {
        if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        {
            return {vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear};
        }

        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace ==
                vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR VulkanRenderer::chooseSwapPresentMode(
        const std::vector<vk::PresentModeKHR>& availablePresentModes) const
    {
        auto bestMode = vk::PresentModeKHR::eFifo;

        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            {
                return availablePresentMode;
            }
            else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
            {
                bestMode = availablePresentMode;
            }
        }

        return bestMode;
    }

    vk::Extent2D VulkanRenderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width;
            int height;
            glfwGetFramebufferSize(glfwWindow, &width, &height);
            vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);
            return actualExtent;
        }
    }

    void VulkanRenderer::createImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); ++i)
        {
            swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat,
                                                     vk::ImageAspectFlagBits::eColor);
        }
    }

    void VulkanRenderer::createRenderPass()
    {
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        // TODO: ImGui renders final layout, not this pass!
        //colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentDescription depthAttachment = {};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference depthAttachmentRef;
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::SubpassDescription subPass = {};
        subPass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subPass.colorAttachmentCount = 1;
        subPass.pColorAttachments = &colorAttachmentRef;
        subPass.pDepthStencilAttachment = &depthAttachmentRef;

        vk::SubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.srcAccessMask = vk::AccessFlagBits::eNone;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite |
            vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        const std::array attachments = {colorAttachment, depthAttachment};
        vk::RenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subPass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        try
        {
            renderPass = logicalDevice->createRenderPass(renderPassInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create render pass: " + std::string(e.what()));
        }
    }

    vk::UniqueShaderModule VulkanRenderer::createShaderModule(const std::vector<char>& shaderCode)
    {
        try
        {
            return logicalDevice->createShaderModuleUnique({
                vk::ShaderModuleCreateFlags(),
                shaderCode.size(),
                reinterpret_cast<const uint32_t*>(shaderCode.data())
            });
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create shader module: " + std::string(e.what()));
        }
    }

    void VulkanRenderer::createDescriptorSetLayout()
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding;
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        vk::DescriptorSetLayoutBinding samplerLayoutBinding;
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        const std::array bindings = {uboLayoutBinding, samplerLayoutBinding};

        vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        try
        {
            descriptorSetLayout = logicalDevice->createDescriptorSetLayout(layoutInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create descriptor set layout: " + std::string(e.what()));
        }
    }

    void VulkanRenderer::createGraphicsPipeline()
    {
        const auto assetManager = Utility::ServiceLocator::getService<Assets::AssetManager>();
        const auto vertexShaderAsset = assetManager->getAsset<Assets::ShaderAsset>("Assets/Shaders/shader.vert.spv");
        const auto fragmentShaderAsset = assetManager->getAsset<Assets::ShaderAsset>("Assets/Shaders/shader.frag.spv");
        if (!vertexShaderAsset || !fragmentShaderAsset)
        {
            throw std::runtime_error("Failed to load shader assets!");
        }

        auto vertexShaderModule = createShaderModule(vertexShaderAsset->getShader());
        auto fragmentShaderModule = createShaderModule(fragmentShaderAsset->getShader());
        std::array shaderStages = {
            vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex,
                                              *vertexShaderModule, "main"),
            vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment,
                                              *fragmentShaderModule, "main")
        };

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        vk::Viewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChainExtent.width);
        viewport.height = static_cast<float>(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor = {};
        scissor.offset = vk::Offset2D(0, 0);
        scissor.extent = swapChainExtent;

        vk::PipelineViewportStateCreateInfo viewportState = {};
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        vk::PipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizer.depthBiasEnable = VK_FALSE;

        vk::PipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

        vk::PipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = vk::CompareOp::eLess;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = vk::StencilOpState();
        depthStencil.back = vk::StencilOpState();

        vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;

        vk::PipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::array<vk::PushConstantRange, 1> pushConstantRanges = {};

        vk::PushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantObject);

        pushConstantRanges[0] = pushConstantRange;

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

        try
        {
            pipelineLayout = logicalDevice->createPipelineLayout(pipelineLayoutInfo);
        }
        catch (const vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create pipeline layout: " + std::string(e.what()));
        }

        vk::GraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr;
        auto graphicsPipelineResult = logicalDevice->createGraphicsPipeline(nullptr, pipelineInfo);
        if (graphicsPipelineResult.result == vk::Result::eSuccess)
        {
            graphicsPipeline = graphicsPipelineResult.value;
        }
        else
        {
            throw std::runtime_error(
                "Failed to create graphics pipeline, error: " + vk::to_string(graphicsPipelineResult.result));
        }
    }

    void VulkanRenderer::createFramebuffers()
    {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); ++i)
        {
            const std::array attachments = {
                swapChainImageViews[i],
                depthImageView
            };

            vk::FramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            try
            {
                swapChainFramebuffers[i] = logicalDevice->createFramebuffer(framebufferInfo);
            }
            catch (vk::SystemError& e)
            {
                throw std::runtime_error("Failed to create framebuffer: " + std::string(e.what()));
            }
        }
    }

    void VulkanRenderer::createCommandPools()
    {
        globalCommandPool = createCommandPool();
        for (size_t i = 0; i < swapChainImages.size(); ++i)
        {
            commandPools.push_back(createCommandPool());
        }
    }

    vk::CommandPool VulkanRenderer::createCommandPool()
    {
        const QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        try
        {
            return logicalDevice->createCommandPool(poolInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create commandpool: " + std::string(e.what()));
        }
    }

    void VulkanRenderer::createDepthResources()
    {
        const auto depthFormat = findDepthFormat();
        createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal,
                    depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
        transitionImageLayout(depthImage, depthFormat, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    vk::Format VulkanRenderer::findSupportedFormat(const std::vector<vk::Format>& candidates,
                                                   const vk::ImageTiling tiling,
                                                   const vk::FormatFeatureFlags& features) const
    {
        for (const auto format : candidates)
        {
            vk::FormatProperties properties;
            properties = physicalDevice.getFormatProperties(format);
            if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
            {
                return format;
            }
            if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error("Failed to find supported format!");
    }

    vk::Format VulkanRenderer::findDepthFormat() const
    {
        return findSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                                   vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    bool VulkanRenderer::hasStencilComponent(const vk::Format format)
    {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }

    //TODO: Load texture from actor, not hardcoded
    void VulkanRenderer::createTextureImage()
    {
        const auto assetManager = Utility::ServiceLocator::getService<Assets::AssetManager>();
        const auto textureAsset = assetManager->getAsset<Assets::TextureAsset>("Assets/Textures/texture.jpg");
        if (!textureAsset)
        {
            throw std::runtime_error("Failed to load texture asset!");
        }
        const vk::DeviceSize bytesPerPixel = (static_cast<vk::DeviceSize>(textureAsset->getChannels()) * textureAsset->
            getBitsPerChannel()) / 8;
        const vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(textureAsset->getWidth()) * textureAsset->
            getHeight() * (bytesPerPixel + 1);

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;

        createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer, stagingBufferMemory);
        void* data = logicalDevice->mapMemory(stagingBufferMemory, 0, imageSize);
        memcpy(data, textureAsset->getTexture().get(), imageSize);
        logicalDevice->unmapMemory(stagingBufferMemory);

        createImage(textureAsset->getWidth(), textureAsset->getHeight(), vk::Format::eR8G8B8A8Srgb,
                    vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                    vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

        transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eTransferDstOptimal);
        copyBufferToImage(stagingBuffer, textureImage, textureAsset->getWidth(), textureAsset->getHeight());
        transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal,
                              vk::ImageLayout::eShaderReadOnlyOptimal);

        logicalDevice->destroyBuffer(stagingBuffer);
        logicalDevice->freeMemory(stagingBufferMemory);
    }

    void VulkanRenderer::createImage(const uint32_t width, const uint32_t height, const vk::Format format,
                                     const vk::ImageTiling tiling, const vk::ImageUsageFlags usage,
                                     const vk::MemoryPropertyFlags properties, vk::Image& image,
                                     vk::DeviceMemory& imageMemory)
    {
        vk::ImageCreateInfo imageInfo = {};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = usage;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;
        imageInfo.samples = vk::SampleCountFlagBits::e1;

        const auto createImageResult = logicalDevice->createImage(&imageInfo, nullptr, &image);
        if (createImageResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create texture image, error: " + vk::to_string(createImageResult));
        }

        vk::MemoryRequirements memRequirements;
        logicalDevice->getImageMemoryRequirements(image, &memRequirements);
        vk::MemoryAllocateInfo allocInfo = {};
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
        const auto memoryAllocResult = logicalDevice->allocateMemory(&allocInfo, nullptr, &imageMemory);
        if (memoryAllocResult != vk::Result::eSuccess)
        {
            throw std::runtime_error(
                "Failed to allocate texture image memory, error: " + vk::to_string(memoryAllocResult));
        }

        logicalDevice->bindImageMemory(image, imageMemory, 0);
    }

    void VulkanRenderer::createTextureImageView()
    {
        textureImageView = createImageView(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
    }

    vk::ImageView VulkanRenderer::createImageView(const vk::Image& image, const vk::Format& format,
                                                  const vk::ImageAspectFlags& aspectFlags)
    {
        vk::ImageViewCreateInfo viewInfo = {};
        viewInfo.image = image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vk::ImageView imageView;
        const auto createImageViewResult = logicalDevice->createImageView(&viewInfo, nullptr, &imageView);
        if (createImageViewResult != vk::Result::eSuccess)
        {
            throw std::runtime_error(
                "Failed to create texture image view, error: " + vk::to_string(createImageViewResult));
        }

        return imageView;
    }

    void VulkanRenderer::createTextureSampler()
    {
        vk::SamplerCreateInfo samplerInfo = {};
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.anisotropyEnable = VK_TRUE;
        const auto deviceProperties = physicalDevice.getProperties();
        samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = vk::CompareOp::eAlways;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        const auto createSamplerResult = logicalDevice->createSampler(&samplerInfo, nullptr, &textureSampler);
        if (createSamplerResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create texture sampler, error: " + vk::to_string(createSamplerResult));
        }
    }

    vk::CommandBuffer VulkanRenderer::beginSingleTimeCommands()
    {
        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = globalCommandPool;
        allocInfo.commandBufferCount = 1;

        const vk::CommandBuffer commandBuffer = logicalDevice->allocateCommandBuffers(allocInfo)[0];

        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);
        return commandBuffer;
    }

    void VulkanRenderer::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo = {};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        graphicsQueue.submit(submitInfo, nullptr);
        graphicsQueue.waitIdle();

        logicalDevice->freeCommandBuffers(globalCommandPool, commandBuffer);
    }

    void VulkanRenderer::transitionImageLayout(vk::Image image, vk::Format format, const vk::ImageLayout oldLayout,
                                               const vk::ImageLayout newLayout)
    {
        const vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        vk::ImageMemoryBarrier barrier = {};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
            if (hasStencilComponent(format))
            {
                barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        }

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eNone;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout ==
            vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout ==
            vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eNone;
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), nullptr, nullptr, barrier);

        endSingleTimeCommands(commandBuffer);
    }

    void VulkanRenderer::copyBufferToImage(vk::Buffer buffer, vk::Image image, const uint32_t width,
                                           const uint32_t height)
    {
        const vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        vk::BufferImageCopy region;
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = vk::Offset3D{0, 0, 0};
        region.imageExtent = vk::Extent3D{width, height, 1};

        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

        endSingleTimeCommands(commandBuffer);
    }

    void VulkanRenderer::createUniformBuffers()
    {
        uniformBuffers.resize(maxFramesInFlight);
        uniformBuffersMemory.resize(maxFramesInFlight);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            constexpr vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
            createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                         uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }

    void VulkanRenderer::createBuffer(const vk::DeviceSize size, const vk::BufferUsageFlags& usage,
                                      const vk::MemoryPropertyFlags& properties, vk::Buffer& buffer,
                                      vk::DeviceMemory& bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        try
        {
            buffer = logicalDevice->createBuffer(bufferInfo);
        }
        catch (vk::SystemError& err)
        {
            throw std::runtime_error("failed to create buffer: " + std::string(err.what()));
        }

        const vk::MemoryRequirements memRequirements = logicalDevice->getBufferMemoryRequirements(buffer);

        vk::MemoryAllocateInfo allocInfo = {};
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        try
        {
            bufferMemory = logicalDevice->allocateMemory(allocInfo);
        }
        catch (vk::SystemError& err)
        {
            throw std::runtime_error("failed to allocate buffer memory: " + std::string(err.what()));
        }

        logicalDevice->bindBufferMemory(buffer, bufferMemory, 0);
    }

    void VulkanRenderer::copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize size,
                                    vk::CommandBuffer commandBuffer)
    {
        // If no commandBuffer was supplied, create a new one and end that at the end of this function
        const bool isCommandBufferSupplied = commandBuffer != nullptr;
        if (!isCommandBufferSupplied)
        {
            commandBuffer = beginSingleTimeCommands();
        }


        vk::BufferCopy copyRegion;
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

        if (!isCommandBufferSupplied)
        {
            endSingleTimeCommands(commandBuffer);
        }
    }

    uint32_t VulkanRenderer::findMemoryType(const uint32_t typeFilter, const vk::MemoryPropertyFlags& properties) const
    {
        const vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }

    void VulkanRenderer::createSyncObjects()
    {
        imageAvailableSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);
        inFlightFences.resize(maxFramesInFlight);

        try
        {
            for (size_t i = 0; i < maxFramesInFlight; ++i)
            {
                imageAvailableSemaphores[i] = logicalDevice->createSemaphore({});
                renderFinishedSemaphores[i] = logicalDevice->createSemaphore({});
                inFlightFences[i] = logicalDevice->createFence({vk::FenceCreateFlagBits::eSignaled});
            }
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame: " + std::string(e.what()));
        }
    }

    void VulkanRenderer::createCommandBuffers()
    {
        commandBuffers.resize(swapChainFramebuffers.size());
        for (size_t i = 0; i < swapChainFramebuffers.size(); ++i)
        {
            vk::CommandBufferAllocateInfo allocInfo = {};
            allocInfo.commandPool = commandPools[i];
            allocInfo.level = vk::CommandBufferLevel::ePrimary;
            allocInfo.commandBufferCount = 1;

            try
            {
                commandBuffers[i] = logicalDevice->allocateCommandBuffers(allocInfo)[0];
            }
            catch (vk::SystemError& e)
            {
                throw std::runtime_error("Failed to allocate commandbuffers: " + std::string(e.what()));
            }
        }
    }

    void VulkanRenderer::createCommandBuffer(const vk::CommandBuffer& commandBuffer, const uint32_t currentImage)
    {
        const auto activeScene = sceneManager->getActiveScene();

        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        try
        {
            commandBuffer.begin(beginInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to begin recording commandbuffer: " + std::string(e.what()));
        }

        vk::RenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[currentImage];
        renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<vk::ClearValue, 2> clearValues = {};
        clearValues[0].color = vk::ClearColorValue(std::array{0.0f, 0.0f, 0.0f, 1.0f});
        clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        const auto activeCamera = cameraManager->getActiveCamera();


        commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        {
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
            const auto staticMeshComponents = activeScene->getComponents<Core::StaticMeshComponent>();
            for (const auto& staticMeshComponent : staticMeshComponents)
            {
                if (!staticMeshComponent || !staticMeshComponent->isVisible())
                {
                    continue;
                }
                const auto staticMesh = staticMeshComponent->getStaticMesh();

                const auto meshIter = std::ranges::find_if(meshes,
                                                           [&](const std::shared_ptr<VulkanMesh>& vulkanMesh)
                                                           {
                                                               return vulkanMesh->hasMeshIdAssociated(staticMesh->
                                                                   getMeshId());
                                                           });
                if (meshIter == meshes.end())
                {
                    LOG_ERROR("Could not find associated VulkanMesh of StaticMeshComponent {}, this should not happen!",
                              staticMeshComponent->getName());
                    continue;
                }
                const auto vulkanMesh = *meshIter;
                std::array vertexBuffers = {
                    vulkanMesh->getVertexBuffer().getBuffer()
                };
                constexpr std::array<vk::DeviceSize, 1> offsets = {0};
                const auto indexBuffer = vulkanMesh->getIndexBuffer().getBuffer();
                commandBuffer.bindVertexBuffers(0, 1, vertexBuffers.data(), offsets.data());
                commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
                commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1,
                                                 &descriptorSets[currentFrame], 0, nullptr);

                PushConstantObject pco = {};
                pco.model = staticMeshComponent->getAbsoluteTransform().toMatrix();
                pco.color = staticMeshComponent->getMeshColor();
                if (activeCamera)
                {
                    pco.lightPos = activeCamera->getAbsoluteTransform().getTranslation();
                }
                else
                {
                    pco.lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
                }
                commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0,
                                            sizeof(PushConstantObject), &pco);

                commandBuffer.drawIndexed(
                    static_cast<uint32_t>(staticMeshComponent->getStaticMeshAsset()->getIndices().size()), 1, 0, 0,
                    0);
            }
        }

        // ImGui Rendering
        imGuiImpl.newFrame();
        imGuiImpl.render(commandBuffer);

        commandBuffer.endRenderPass();

        try
        {
            commandBuffer.end();
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to end recording commandbuffer: " + std::string(e.what()));
        }
    }

    void VulkanRenderer::updateCommandBuffer(const uint32_t currentImage, const uint32_t imageIndex)
    {
        const auto& oldCommandPool = commandPools[currentImage];
        logicalDevice->resetCommandPool(oldCommandPool);

        createCommandBuffer(commandBuffers[currentImage], imageIndex);
    }

    void VulkanRenderer::updateUniformBuffer(const uint32_t currentImage)
    {
        UniformBufferObject ubo;

        //Retrieve active camera
        if (const auto activeCamera = cameraManager->getActiveCamera())
        {
            auto camTransform = activeCamera->getAbsoluteTransform();
            ubo.view = camTransform.toMatrix(true);
            ubo.projection = glm::perspective(glm::radians(activeCamera->fov),
                                              static_cast<float>(swapChainExtent.width) / static_cast<float>(
                                                  swapChainExtent.height),
                                              activeCamera->zNear, activeCamera->zFar);
        }
        else
        {
            ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
            ubo.projection = glm::perspective(glm::radians(45.0f),
                                              static_cast<float>(swapChainExtent.width) / static_cast<float>(
                                                  swapChainExtent.height), 0.1f,
                                              10.0f);
        }

        constexpr vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
        void* data = logicalDevice->mapMemory(uniformBuffersMemory[currentImage], 0, bufferSize);
        memcpy(data, &ubo, bufferSize);
        logicalDevice->unmapMemory(uniformBuffersMemory[currentImage]);
    }

    void VulkanRenderer::createDescriptorPool()
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = maxFramesInFlight;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = maxFramesInFlight;

        vk::DescriptorPoolCreateInfo poolInfo = {};
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = maxFramesInFlight;

        try
        {
            descriptorPool = logicalDevice->createDescriptorPool(poolInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to create descriptor pool: " + std::string(e.what()));
        }
    }

    void VulkanRenderer::createDescriptorSets()
    {
        const std::vector layouts(maxFramesInFlight, descriptorSetLayout);
        vk::DescriptorSetAllocateInfo allocInfo = {};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = maxFramesInFlight;
        allocInfo.pSetLayouts = layouts.data();

        try
        {
            descriptorSets = logicalDevice->allocateDescriptorSets(allocInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("Failed to allocate descriptor sets: " + std::string(e.what()));
        }

        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            constexpr vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
            vk::DescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = bufferSize;

            vk::DescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfo.imageView = textureImageView;
            imageInfo.sampler = textureSampler;

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr;
            descriptorWrites[0].pTexelBufferView = nullptr;

            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            logicalDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                                                0, nullptr);
        }
    }
}
