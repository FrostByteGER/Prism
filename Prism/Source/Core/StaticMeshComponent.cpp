#include "StaticMeshComponent.hpp"
#include "../Utilities/ServiceLocator.hpp"
#include "MeshFactoryRegistry.hpp"
#include "StaticMeshFactory.hpp"
#include "../Rendering/IRendererManager.hpp"

Prism::Core::StaticMeshComponent::~StaticMeshComponent()
{
    if (staticMesh)
    {
        Utility::ServiceLocator::getService<Rendering::IRendererManager>()->getRenderer()->
                                                                            unregisterStaticMesh(
                                                                                staticMesh->getMeshId());
    }
}

void Prism::Core::StaticMeshComponent::init()
{
    ActorComponent::init();
    const auto staticMeshFactory = Utility::ServiceLocator::getService<MeshFactoryRegistry>()->
        getMeshFactory<StaticMeshFactory>();
    staticMesh = staticMeshFactory->createMesh(staticMeshAsset);
}

void Prism::Core::StaticMeshComponent::initDeferred()
{
    ActorComponent::initDeferred();
}

void Prism::Core::StaticMeshComponent::shutdown()
{
    ActorComponent::shutdown();
}
