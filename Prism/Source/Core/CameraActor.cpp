#include "CameraActor.hpp"

#include "IEngineManager.hpp"
#include "../Utilities/ServiceLocator.hpp"
#include "../Rendering/ICameraManager.hpp"
#include "../Utilities/Globals.hpp"

Prism::Core::CameraActor::CameraActor()
{
    setName("CameraActor");
    cameraComponent = addComponent<CameraComponent>();
}

void Prism::Core::CameraActor::init()
{
    Actor::init();
    const auto cameraManager = Utility::ServiceLocator::getService<Rendering::ICameraManager>();
    cameraManager->setActiveCamera(cameraComponent);
    inputManager = Utility::ServiceLocator::getService<Input::IInputManager>();
    inputManager->registerMouseMovementInput([this](const double xPos, const double yPos)
    {
        onMouseMoved(static_cast<float>(xPos), static_cast<float>(yPos));
    });
}

void Prism::Core::CameraActor::shutdown()
{
    Actor::shutdown();
}

void Prism::Core::CameraActor::tick(const float deltaTime)
{
    if (inputManager->isKeyDown(Input::InputKey::ESCAPE))
    {
        Utility::ServiceLocator::getService<IEngineManager>()->requestShutdown();
        return;
    }

    const float movementSpeed = (inputManager->isKeyDown(Input::InputKey::LEFT_SHIFT) ? 7.0f : 2.5f) * deltaTime;


    auto translation = glm::vec3(0.0f);
    if (inputManager->isKeyDown(Input::InputKey::W))
    {
        translation += this->transform.forward();
    }
    if (inputManager->isKeyDown(Input::InputKey::S))
    {
        translation -= this->transform.forward();
    }
    if (inputManager->isKeyDown(Input::InputKey::A))
    {
        translation -= this->transform.right();
    }
    if (inputManager->isKeyDown(Input::InputKey::D))
    {
        translation += this->transform.right();
    }
    if (inputManager->isKeyDown(Input::InputKey::Q))
    {
        translation -= this->transform.up();
    }
    if (inputManager->isKeyDown(Input::InputKey::E))
    {
        translation += this->transform.up();
    }
    this->transform.translate(movementSpeed * translation);
}

// member function mouse movement callback
void Prism::Core::CameraActor::onMouseMoved(const float xPos, const float yPos)
{
    if (std::abs(mouseLastXPos - -1.0f) <= 1.0e-05f)
    {
        mouseLastXPos = xPos;
    }
    if (std::abs(mouseLastYPos - -1.0f) <= 1.0e-05f)
    {
        mouseLastYPos = yPos;
    }
    const float deltaX = xPos - mouseLastXPos;
    const float deltaY = yPos - mouseLastYPos;

    if (inputManager->isMouseButtonDown(Input::MouseButton::RIGHT))
    {
        constexpr float rotationSpeed = 0.15f;
        this->transform.rotate(rotationSpeed * deltaX, Transform::localUp);
        this->transform.rotate(rotationSpeed * deltaY, this->transform.right());
    }

    mouseLastXPos = xPos;
    mouseLastYPos = yPos;
}
