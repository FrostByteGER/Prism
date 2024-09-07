#pragma once
#include "IWindow.hpp"
#include "../Utilities/IService.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Rendering
{
    class IWindowManager : public Utility::IService
    {
    public:
        ~IWindowManager() override = default;
        [[nodiscard]] virtual RawPtr<IWindow> getWindow() const = 0;

        virtual void setWindow(std::unique_ptr<IWindow>&& newWindow) = 0;

        virtual void initialize() override = 0;
        virtual void initializeDeferred() override = 0;
        virtual void deInitialize() override = 0;
        virtual std::string getFullName() override = 0;
    };
}
