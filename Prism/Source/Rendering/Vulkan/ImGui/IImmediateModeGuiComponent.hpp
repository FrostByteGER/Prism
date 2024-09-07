#pragma once

namespace Prism::Rendering
{
    class IImmediateModeGuiComponent
    {
    public:
        virtual ~IImmediateModeGuiComponent() = default;
        virtual void renderUi() = 0;
        virtual void shutdown() = 0;
    };
}
