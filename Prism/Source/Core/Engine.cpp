#include "Engine.hpp"

#include "IEngineManager.hpp"
#include "../Utilities/Logging/Log.hpp"
#include "../Utilities/ServiceLocator.hpp"
#include "../Rendering/IWindowManager.hpp"
#include "../Input/IInputManager.hpp"
#include "../Rendering/GlfwWindow.hpp"
#include <format>


Prism::Core::Engine::Engine(BaseBootstrapper* bootstrapper) : bootstrapper(
    std::unique_ptr<BaseBootstrapper>(bootstrapper))
{
}

void Prism::Core::Engine::setup(const std::vector<Utility::CommandLineArg>& commandLineArgs)
{
    bootstrapper->bootstrapInternal(commandLineArgs);
}

void Prism::Core::Engine::start()
{
    sceneManager = Utility::ServiceLocator::getService<ISceneManager>();
    const auto windowManager = Utility::ServiceLocator::getService<Rendering::IWindowManager>();
    windowManager->setWindow(std::make_unique<Rendering::GlfwWindow>());
    const auto window = windowManager->getWindow();
    window->init();
    Utility::ServiceLocator::deferredInitializeServicesInternal();
    // Take ownership of scene
    sceneManager->loadScene(std::unique_ptr<Scene>(bootstrapper->getDefaultScene().get()));
    const auto inputManager = Utility::ServiceLocator::getService<Input::IInputManager>();
    const auto engineManager = Utility::ServiceLocator::getService<IEngineManager>();
    while (!window->isShutdownRequested() && !engineManager->isShutdownRequested())
    {
        // Update frame delta first, then fetch it
        engineClock.updateFrameDelta();
        frameDeltaSeconds = engineClock.getFrameDelta();
        const float frameDeltaMs = engineClock.getFrameDeltaMilliseconds();
        frameAccumulatorSeconds += frameDeltaSeconds;
        if (frameAccumulatorSeconds >= 1.0f)
        {
            framesPerSecond = static_cast<float>(engineClock.getFrameCount()) / frameAccumulatorSeconds;
            const auto info = std::format("FPS: {} | Frame: {}ms | Render: {}ms | Update: {}ms | Physics: {}ms",
                                          framesPerSecond, frameDeltaMs, engineClock.getRenderDeltaMilliseconds(),
                                          engineClock.getUpdateDeltaMilliseconds(),
                                          engineClock.getPhysicsDeltaMilliseconds());
            //LOG_DEBUG(info);
            window->setWindowTitle(info);
            frameAccumulatorSeconds = 0.0f;
            engineClock.reset();
        }
        window->pollWindowEvents();
        engineClock.startUpdateTimer();
        sceneManager->tick(frameDeltaSeconds);
        engineClock.stopUpdateTimer();

        engineClock.startRenderTimer();
        window->tick(frameDeltaSeconds);
        engineClock.stopRenderTimer();
    }
    LOG_DEBUG("Shutdown requested...");
    //TODO: Move to shutdown or manage otherwise like in WindowManager?
    window->shutdown();
}

void Prism::Core::Engine::shutdown()
{
    LOG_DEBUG("Shutting down engine...");
    bootstrapper->shutdownInternal();
}
