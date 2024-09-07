#pragma once
#include <string>

#include "IWindow.hpp"
#include "IRendererManager.hpp"
#include "../Utilities/Globals.hpp"

struct GLFWwindow;

namespace Prism::Rendering
{
    class GlfwWindow : public IWindow
    {
    public:
        ~GlfwWindow() override = default;
        void init() override;
        void tick(float deltaTime) override;
        void shutdown() override;
        bool isShutdownRequested() override;
        void pollWindowEvents() override;
        void setWindowTitle(const std::string& title) override;
        static void onFrameBufferResized(GLFWwindow* window, int width, int height);

        GLFWwindow* getGLFWWindow() const
        {
            return window;
        }

    private:
        GLFWwindow* window = nullptr;
        RawPtr<IRenderer> renderer;
        RawPtr<IRendererManager> rendererManager;
    };
}
