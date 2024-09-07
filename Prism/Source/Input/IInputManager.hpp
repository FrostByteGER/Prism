#pragma once
#include "../Utilities/IService.hpp"
#include "../Rendering/WindowManager.hpp"
#include "InputKey.hpp"
#include "MouseButton.hpp"

namespace Prism::Input
{
    class IInputManager : public Utility::IService
    {
    public:
        explicit IInputManager(const RawPtr<Rendering::IWindowManager> windowManager) : windowManager(windowManager)
        {
        }

        ~IInputManager() override = default;
        virtual void initialize() override = 0;
        virtual void initializeDeferred() override = 0;
        virtual void initCursorCallback() = 0;
        virtual void deInitialize() override = 0;
        virtual void registerMouseMovementInput(const std::function<void(double xPos, double yPos)>& callback) = 0;

        [[nodiscard]] virtual bool isKeyDown(InputKey key) const = 0;
        [[nodiscard]] virtual bool isKeyPressed(InputKey key) const = 0;
        [[nodiscard]] virtual bool isKeyReleased(InputKey key) const = 0;
        [[nodiscard]] virtual bool isMouseButtonDown(MouseButton) const = 0;
        [[nodiscard]] virtual bool isMouseButtonReleased(MouseButton) const = 0;

        virtual void cleanupDeadCallbacks() = 0;

        [[nodiscard]] bool isCleanupDesired() const
        {
            return deadMouseCallbacks >= deadCallbacksThreshold;
        }

        [[nodiscard]] size_t getDeadCallbacksThreshold() const
        {
            return deadCallbacksThreshold;
        }

        void setDeadCallbacksThreshold(const size_t newValue)
        {
            deadCallbacksThreshold = newValue;
        }

        virtual std::string getFullName() override = 0;

        [[nodiscard]] std::vector<std::function<void(double, double)>> getMouseMovementCallbacksInternal() const
        {
            return mouseMovementCallbacks;
        }

        void setMouseMovementCallbacksInternal(
            const std::vector<std::function<void(double, double)>>& newMouseMovementCallbacks)
        {
            this->mouseMovementCallbacks = newMouseMovementCallbacks;
        }

        [[nodiscard]] size_t getDeadMouseCallbacksInternal() const
        {
            return deadMouseCallbacks;
        }

        void setDeadMouseCallbacksInternal(const size_t newDeadMouseCallbacks)
        {
            this->deadMouseCallbacks = newDeadMouseCallbacks;
        }

    protected:
        RawPtr<Rendering::IWindowManager> windowManager;
        std::vector<std::function<void(double, double)>> mouseMovementCallbacks;
        size_t deadMouseCallbacks;
        size_t deadCallbacksThreshold;
    };
}
