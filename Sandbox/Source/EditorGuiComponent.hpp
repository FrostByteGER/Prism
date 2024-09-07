#pragma once
#include "Rendering/Vulkan/ImGui/IImmediateModeGuiComponent.hpp"
#include "Core/Actor.hpp"

class EditorGuiComponent : public Prism::Rendering::IImmediateModeGuiComponent
{
public:
    EditorGuiComponent() = default;
    void renderUi() override;
    void renderSelectedActorInfo(const RawPtr<Prism::Core::Actor> actor);
    void renderGraphicsDebugBridgeInfo();
    void shutdown() override;

private:
    RawPtr<Prism::Core::Actor> selectedActor;
};
