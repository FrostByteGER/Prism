#pragma once
#include "IGraphicsDebugBridgeImpl.hpp"
#ifdef INCLUDE_NGFX_HEADERS
#include <NGFX_Injection.h>
#endif
#include <string>
#ifdef INCLUDE_NGFX_HEADERS
namespace Prism::Rendering
{
    class GraphicsDebugBridgeImplNvidiaNsightGraphics : public IGraphicsDebugBridgeImpl
    {
    public:
        bool initialize(const GraphicsDebugBridgeActivityType& activityType) override;
        void deInitialize() override;
        bool startSingleFrameCapture() override;
        void startFrameCapture() override;
        void endFrameCapture() override;

    private:

        [[nodiscard]] std::string convertNGFX_Injection_ResultToString(const NGFX_Injection_Result& result) const;
        [[nodiscard]] std::string convertNGFX_Injection_ActivityTypeToString(
            const NGFX_Injection_ActivityType& activityType) const;
    };
}
#endif