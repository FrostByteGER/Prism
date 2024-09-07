#include "SandboxScene.hpp"

#include <filesystem>
#include <glm/gtc/random.hpp>

#include "Assets/AssetManager.hpp"
#include "Core/CameraActor.hpp"
#include "Core/StaticMeshActor.hpp"
#include "Rendering/DebugDrawHelper.hpp"
#include "EditorGuiComponent.hpp"
#include "Rendering/IRendererManager.hpp"
#include "Utilities/ServiceLocator.hpp"

SandboxScene::SandboxScene()
{
}

SandboxScene::~SandboxScene()
{
}

void SandboxScene::tick(float deltaTime)
{
    Scene::tick(deltaTime);
}

void SandboxScene::init()
{
    Scene::init();
    const auto cameraActor = registerActor(std::make_unique<Prism::Core::CameraActor>());
    const auto staticMeshActor = registerActor(std::make_unique<Prism::Core::StaticMeshActor>());
    const auto staticMeshAsset = Prism::Utility::ServiceLocator::getService<Prism::Assets::AssetManager>()->getAsset
        <Prism::Assets::StaticMeshAsset>("Assets/StaticMeshes/Crate/Crate.obj");
    const auto staticMeshComp = staticMeshActor->getFirstComponentOfType<Prism::Core::StaticMeshComponent>();
    staticMeshComp->setMeshColor(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)));
    staticMeshComp->setStaticMeshAsset(staticMeshAsset);
    cameraActor->setActorPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    const auto debugDrawHelper = registerActor(std::make_unique<Prism::Rendering::DebugDrawHelper>());
}

void SandboxScene::beginPlay()
{
    Scene::beginPlay();
}

void SandboxScene::shutdown()
{
    Scene::shutdown();
}

void SandboxScene::initGuiComponents()
{
    Scene::initGuiComponents();
    const auto renderer = Prism::Utility::ServiceLocator::getService<Prism::Rendering::IRendererManager>()->
        getRenderer();
    renderer->getImmediateModeGui()->addImmediateModeGuiComponent<EditorGuiComponent>();
}
