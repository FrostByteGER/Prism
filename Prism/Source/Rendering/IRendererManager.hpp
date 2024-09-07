#pragma once
#include "../Utilities/IService.hpp"
#include "IRenderer.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Rendering
{
    class IRendererManager : public Utility::IService
    {
    public:
        virtual ~IRendererManager() override = default;
        [[nodiscard]] virtual RawPtr<IRenderer> getRenderer() const = 0;

        virtual void setActiveRenderer(std::unique_ptr<IRenderer>&& newRenderer) = 0;
    };
}
