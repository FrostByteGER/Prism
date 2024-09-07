#include "GraphicsDebugBridge.hpp"
#ifdef _WIN64
#ifdef INCLUDE_NGFX_HEADERS
#include "GraphicsDebugBridgeImplNvidiaNsightGraphics.hpp"
#endif
#endif
#include "../Utilities/Logging/Log.hpp"

void Prism::Rendering::GraphicsDebugBridge::initBridge(const GraphicsDebugBridgeType& bridgeType,
                                                         const GraphicsDebugBridgeActivityType& activityType)
{
    //TODO: Replace switch with better solution!
    switch (bridgeType)
    {
    case GraphicsDebugBridgeType::NVIDIA_NSIGHT_GRAPHICS:
#ifdef _WIN64
#ifdef INCLUDE_NGFX_HEADERS
        bridgeImpl = std::make_unique<GraphicsDebugBridgeImplNvidiaNsightGraphics>();
        break;
#endif
#else
        LOG_WARN("Nvidia NSight Graphics SDK is not supported on non-windows systems!");
        return;
#endif
    case GraphicsDebugBridgeType::RENDERDOC:
        LOG_WARN("RenderDoc is not yet supported!");
        return;
    case GraphicsDebugBridgeType::NONE:
        LOG_INFO("No graphics debug bridge selected!");
        return;
    }
    initialized = bridgeImpl->initialize(activityType);
}

bool Prism::Rendering::GraphicsDebugBridge::startSingleFrameCapture()
{
    if (!bridgeImpl)
    {
        return false;
    }
    if (!initialized)
    {
        LOG_WARN("Cannot start single frame capture, bridge not initialized!");
        return false;
    }
    if (capturing)
    {
        LOG_WARN("Already capturing a frame!");
        return false;
    }
    return bridgeImpl->startSingleFrameCapture();
}

void Prism::Rendering::GraphicsDebugBridge::startFrameCapture()
{
    if (!bridgeImpl)
    {
        return;
    }
    if (!initialized)
    {
        LOG_WARN("Cannot start frame capture, bridge not initialized!");
        return;
    }
    if (capturing)
    {
        LOG_WARN("Already capturing a frame!");
        return;
    }
    capturing = true;
    bridgeImpl->startFrameCapture();
}

void Prism::Rendering::GraphicsDebugBridge::endFrameCapture()
{
    if (!bridgeImpl || !initialized || !capturing)
    {
        return;
    }
    capturing = false;
    bridgeImpl->endFrameCapture();
}

void Prism::Rendering::GraphicsDebugBridge::deInitBridge()
{
    if (!bridgeImpl)
    {
        return;
    }
    bridgeImpl->deInitialize();
}
