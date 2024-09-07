#pragma once

#include "IInputManager.hpp"
#include "MouseButton.hpp"
#include "../Utilities/Globals.hpp"

struct GLFWwindow;

namespace Prism::Input
{
    class GlfwInputManager : public IInputManager
    {
    public:
        explicit GlfwInputManager(RawPtr<Rendering::IWindowManager> windowManager);
        ~GlfwInputManager() override = default;
        void initialize() override;
        void initializeDeferred() override;
        void initCursorCallback() override;
        void deInitialize() override;
        void registerMouseMovementInput(const std::function<void(double xPos, double yPos)>& callback) override;

        [[nodiscard]] bool isKeyDown(InputKey key) const override;
        [[nodiscard]] bool isKeyPressed(InputKey key) const override;
        [[nodiscard]] bool isKeyReleased(InputKey key) const override;
        [[nodiscard]] bool isMouseButtonDown(MouseButton button) const override;
        [[nodiscard]] bool isMouseButtonReleased(MouseButton button) const override;

        void cleanupDeadCallbacks() override;

        std::string getFullName() override
        {
            return "Prism::Input::GlfwInputManager";
        }

    private:
        GLFWwindow* glfwWindow;
        std::array<int, 16> mouseButtonStates;
        std::array<bool, 512> activeMouseButtons;
        long lastMouseNS = 0;
        long mouseDoubleClickPeriodNS = 1000000000 / 5; //5th of a second for double click

        InputKey mapGlfwKeyToInputKey(int glfwKey) const;
        int mapInputKeyToGlfwKey(InputKey inputKey) const;
        int mapMouseButtonToGlfwButton(MouseButton button) const;
    };
}
