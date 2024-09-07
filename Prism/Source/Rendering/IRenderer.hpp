#pragma once
#include "Vulkan/ImGui/AbstractImmediateModeGui.hpp"
#include "../Core/StaticMesh.hpp"

namespace Prism::Rendering
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;
        virtual void init() = 0;
        virtual void render() = 0;
        virtual void shutdown() = 0;
        virtual RawPtr<AbstractImmediateModeGui> getImmediateModeGui() = 0;
        virtual void onFrameBufferResized(int width, int height) = 0;
        virtual void registerNewStaticMesh(RawPtr<Core::StaticMesh> staticMesh) = 0;
        virtual void unregisterStaticMesh(uint64_t staticMeshId) = 0;
    };
}
