#pragma once
#include "Actor.hpp"
#include "CameraComponent.hpp"
#include "../Input/IInputManager.hpp"

namespace Prism::Core
{
    class CameraActor : public Actor
    {
    public:
        CameraActor();
        ~CameraActor() override = default;
        void tick(float deltaTime) override;
        void onMouseMoved(float xPos, float yPos);

    protected:
        void init() override;
        void shutdown() override;

    private:
        RawPtr<CameraComponent> cameraComponent;
        RawPtr<Input::IInputManager> inputManager;
        float mouseLastXPos = -1.0f;
        float mouseLastYPos = -1.0f;
    };
}
