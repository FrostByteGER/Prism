#pragma once
#include "IImmediateModeGuiComponent.hpp"
#include "../../../Utilities/Globals.hpp"

namespace Prism::Rendering
{
    template <typename T>
    concept ExtendsImmediateModeGuiComponent = std::is_base_of_v<IImmediateModeGuiComponent, T>;

    class AbstractImmediateModeGui
    {
    public:
        virtual ~AbstractImmediateModeGui() = default;

        template <ExtendsImmediateModeGuiComponent T>
        RawPtr<T> addImmediateModeGuiComponent()
        {
            auto component = std::make_unique<T>();
            RawPtr<T> returnComponent = RawPtr<T>(component.get());
            guiComponents.emplace_back(std::move(component));
            return returnComponent;
        }

    protected:
        std::vector<std::unique_ptr<IImmediateModeGuiComponent>> guiComponents;
    };
}
