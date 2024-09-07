#pragma once
#include "IWindowManager.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Rendering
{
    class WindowManager : public IWindowManager
    {
    public:
        ~WindowManager() override = default;

        [[nodiscard]] RawPtr<IWindow> getWindow() const override
        {
            return {window};
        }

        void setWindow(std::unique_ptr<IWindow>&& newWindow) override
        {
            window = std::move(newWindow);
        }

        void initialize() override;
        void initializeDeferred() override;
        void deInitialize() override;

        std::string getFullName() override
        {
            return "Prism::Rendering::WindowManager";
        }

    private:
        std::unique_ptr<IWindow> window;
    };
}
