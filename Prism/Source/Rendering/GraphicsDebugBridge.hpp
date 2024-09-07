#pragma once
#include <memory>

#include "IGraphicsDebugBridgeImpl.hpp"
#include "../Utilities/IService.hpp"

namespace Prism::Rendering
{
    enum class GraphicsDebugBridgeType
    {
        NVIDIA_NSIGHT_GRAPHICS,
        RENDERDOC,
        NONE
    };

    class GraphicsDebugBridge : public Utility::IService
    {
    public:
        GraphicsDebugBridge() = default;
        ~GraphicsDebugBridge() override = default;

        void initialize() override
        {
            initBridge(Prism::Rendering::GraphicsDebugBridgeType::NONE,
                       Prism::Rendering::GraphicsDebugBridgeActivityType::GPU_TRACE);
        }

        void initializeDeferred() override
        {
        }

        void deInitialize() override
        {
        }

        std::string getFullName() override
        {
            return "Prism::Rendering::GraphicsDebugBridge";
        }

        bool startSingleFrameCapture();
        void startFrameCapture();
        void endFrameCapture();
        void deInitBridge();

        [[nodiscard]] bool isInitialized() const
        {
            return initialized;
        }

    private:
        std::unique_ptr<IGraphicsDebugBridgeImpl> bridgeImpl;
        bool initialized = false;
        bool capturing = false;

        void initBridge(const GraphicsDebugBridgeType& bridgeType, const GraphicsDebugBridgeActivityType& activityType);
    };
}
