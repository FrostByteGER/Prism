#pragma once
#include <memory>

#include "ICameraManager.hpp"
#include "../Core/CameraActor.hpp"

namespace Prism::Rendering
{
    class CameraManager : public ICameraManager
    {
    public:
        ~CameraManager() override = default;
        void initialize() override;
        void initializeDeferred() override;
        void deInitialize() override;

        [[nodiscard]] RawPtr<Core::CameraComponent> getActiveCamera() const override
        {
            return activeCamera;
        }

        void setActiveCamera(const RawPtr<Core::CameraComponent>& newActiveCamera) override
        {
            this->activeCamera = newActiveCamera;
        }

        std::string getFullName() override
        {
            return "Prism::Rendering::CameraManager";
        }

    private:
        RawPtr<Core::CameraComponent> activeCamera;
    };
}
