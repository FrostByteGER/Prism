#pragma once
#include "GraphicsDebugBridgeActivityType.hpp"

namespace Prism::Rendering
{
    class IGraphicsDebugBridgeImpl
    {
    public:
        virtual ~IGraphicsDebugBridgeImpl() = default;
        virtual bool initialize(const GraphicsDebugBridgeActivityType& activityType) = 0;
        virtual void deInitialize() = 0;
        virtual bool startSingleFrameCapture() = 0;
        virtual void startFrameCapture() = 0;
        virtual void endFrameCapture() = 0;
    };
}
