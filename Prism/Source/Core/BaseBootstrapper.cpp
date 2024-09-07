#include "BaseBootstrapper.hpp"

#include "EngineManager.hpp"
#include "IEngineManager.hpp"
#include "MeshFactoryRegistry.hpp"
#include "TimeManager.hpp"
#include "SceneManager.hpp"
#include "StaticMeshFactory.hpp"
#include "../Utilities/ServiceLocator.hpp"
#include "../Rendering/CameraManager.hpp"
#include "../Rendering/RendererManager.hpp"
#include "../Rendering/WindowManager.hpp"
#include "../Rendering/GraphicsDebugBridge.hpp"
#include "../Input/GlfwInputManager.hpp"
#include "../Assets/AssetManager.hpp"
#include "../Assets/ShaderAssetFactory.hpp"
#include "../Assets/ShaderAsset.hpp"
#include "../Assets/TextureAssetFactory.hpp"
#include "../Assets/TextureAsset.hpp"
#include "../Assets/StaticMeshAssetFactory.hpp"
#include "../Assets/StaticMeshAsset.hpp"
#include "../Utilities/CommandLineArgsManager.hpp"

void Prism::Core::BaseBootstrapper::bootstrapInternal(const std::vector<Utility::CommandLineArg>& commandLineArgs)
{
    const auto seed = std::time(0);
    std::srand(static_cast<unsigned int>(seed));
    LOG_DEBUG("Initialized random number generator 'srand' with seed '{}'", seed);

    using sl = Utility::ServiceLocator;
    sl::registerService<Utility::CommandLineArgsManager>(
        std::make_unique<Utility::CommandLineArgsManager>(commandLineArgs));
    sl::registerService<IEngineManager, EngineManager>();
    sl::registerService<Rendering::GraphicsDebugBridge, Rendering::GraphicsDebugBridge>();
    sl::registerService<ISceneManager, SceneManager>(std::make_unique<SceneManager>());
    sl::registerService<ITimeManager, TimeManager>();
    sl::registerService<Rendering::IRendererManager, Rendering::RendererManager>(
        std::make_unique<Rendering::RendererManager>());
    const auto& meshFactorysRegistry = sl::registerService<MeshFactoryRegistry, MeshFactoryRegistry>();
    meshFactorysRegistry->registerMeshFactory<StaticMeshFactory>();

    const auto& windowManager = sl::registerService<
        Rendering::IWindowManager, Rendering::WindowManager>(std::make_unique<Rendering::WindowManager>());
    sl::registerService<Rendering::ICameraManager, Rendering::CameraManager>();
    sl::registerService<Input::IInputManager, Input::GlfwInputManager>(
        std::make_unique<Input::GlfwInputManager>(windowManager));
    const auto& assetManager = sl::registerService<Assets::AssetManager, Assets::AssetManager>();
    sl::initializeServicesInternal();
    assetManager->registerAssetFactory<Assets::ShaderAssetFactory, Assets::ShaderAsset>();
    assetManager->registerAssetFactory<Assets::TextureAssetFactory, Assets::TextureAsset>();
    assetManager->registerAssetFactory<Assets::StaticMeshAssetFactory, Assets::StaticMeshAsset>();
    bootstrap(commandLineArgs);
}

void Prism::Core::BaseBootstrapper::shutdownInternal()
{
    shutdown();
    Utility::ServiceLocator::deInitializeServicesInternal();
}
