#pragma once
#include <memory>

#include "IRendererManager.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Rendering
{
    class RendererManager : public IRendererManager
    {
    public:
        ~RendererManager() override = default;
        void initialize() override;
        void initializeDeferred() override;
        void deInitialize() override;

        std::string getFullName() override
        {
            return "Prism::Rendering::RendererManager";
        }

        [[nodiscard]] RawPtr<IRenderer> getRenderer() const override
        {
            return renderer;
        }

        void setActiveRenderer(std::unique_ptr<IRenderer>&& newRenderer) override
        {
            renderer = std::move(newRenderer);
        }

        void test()
        {
        }

    private:
        std::unique_ptr<IRenderer> renderer;
    };
}
