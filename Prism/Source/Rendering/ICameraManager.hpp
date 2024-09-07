#pragma once
#include "../Utilities/IService.hpp"
#include "../Core/CameraComponent.hpp"

namespace Prism::Rendering
{
    class ICameraManager : public Utility::IService
    {
    public:
        ~ICameraManager() override = default;
        virtual void initialize() override = 0;
        virtual void initializeDeferred() override = 0;
        virtual void deInitialize() override = 0;

        [[nodiscard]] virtual RawPtr<Core::CameraComponent> getActiveCamera() const = 0;

        virtual void setActiveCamera(const RawPtr<Core::CameraComponent>& newActiveCamera) = 0;

        std::string getFullName() override = 0;
    };
}
