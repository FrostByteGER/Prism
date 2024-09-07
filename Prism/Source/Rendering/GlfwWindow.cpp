#include "GlfwWindow.hpp"
#include "../Utilities/Logging/Log.hpp"
#include "../Utilities/ServiceLocator.hpp"
#include "../Input/IInputManager.hpp"
#include "Vulkan/VulkanRenderer.hpp"
#include <GLFW/glfw3.h>

void Prism::Rendering::GlfwWindow::init()
{
    glfwInit();
    const auto vulkanSupportedResult = glfwVulkanSupported();
    if (vulkanSupportedResult != GLFW_TRUE)
    {
        throw std::runtime_error("Vulkan is not supported with errorcode: " + std::to_string(vulkanSupportedResult));
    }
    LOG_DEBUG("Vulkan is supported!");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1920, 1080, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, onFrameBufferResized);
    const auto inputManager = Utility::ServiceLocator::getService<Input::IInputManager>();
    // Manually initialize cursor callback here as the renderer may init ImGui which will init its own callback which in turn calls our callback.
    // If this step is skipped, the inputmanager will later override the ImGui callback with its own callback which in turn causes ImGui to not work.
    // This may be changed if a MessageBus is implemented.
    inputManager->initCursorCallback();
    rendererManager = Utility::ServiceLocator::getService<IRendererManager>();
    auto vulkanRenderer = std::make_unique<Vulkan::VulkanRenderer>(window);
    renderer = vulkanRenderer.get();
    rendererManager->setActiveRenderer(std::move(vulkanRenderer));
    renderer->init();
}

void Prism::Rendering::GlfwWindow::tick(const float deltaTime)
{
    renderer->render();
}

void Prism::Rendering::GlfwWindow::shutdown()
{
    renderer->shutdown();
    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
}

bool Prism::Rendering::GlfwWindow::isShutdownRequested()
{
    return glfwWindowShouldClose(window);
}

void Prism::Rendering::GlfwWindow::pollWindowEvents()
{
    glfwPollEvents();
}

void Prism::Rendering::GlfwWindow::setWindowTitle(const std::string& title)
{
    glfwSetWindowTitle(window, title.c_str());
}

void Prism::Rendering::GlfwWindow::onFrameBufferResized(GLFWwindow* window, int width, int height)
{
    auto* glfwWindow = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    glfwWindow->renderer->onFrameBufferResized(width, height);
}
