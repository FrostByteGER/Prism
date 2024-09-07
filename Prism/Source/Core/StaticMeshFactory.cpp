#include "StaticMeshFactory.hpp"
#include "../Rendering/IRendererManager.hpp"
#include "../Utilities/ServiceLocator.hpp"

namespace Prism::Core
{
    std::unique_ptr<StaticMesh> StaticMeshFactory::createMesh(
        const RawPtr<Assets::StaticMeshAsset> staticMeshAsset)
    {
        if (!staticMeshAsset)
        {
            return nullptr;
        }
        const auto renderer = Utility::ServiceLocator::getService<Rendering::IRendererManager>()->getRenderer();
        // assign current meshId, then increment it
        auto staticMesh = std::make_unique<StaticMesh>(staticMeshAsset, meshIdCounter);
        ++meshIdCounter;
        // Sanity check, user should looooong run out of VRAM or RAM before this happens.
        if (meshIdCounter == std::numeric_limits<uint64_t>::max())
            LOG_CRITICAL("Mesh ID Counter has reached max value of {}, should not happen ever!", meshIdCounter);
        renderer->registerNewStaticMesh(staticMesh.get());

        return staticMesh;
    }
}
