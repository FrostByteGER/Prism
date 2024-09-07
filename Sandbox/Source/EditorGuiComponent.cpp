#include "EditorGuiComponent.hpp"
#include "Utilities/ServiceLocator.hpp"
#include "Core/ISceneManager.hpp"
#include <glm/gtc/random.hpp>

#include "Assets/AssetManager.hpp"
#include "Rendering/Vulkan/ImGui/imgui.h"

#include "Rendering/DebugDrawHelper.hpp"
#include "Rendering/GraphicsDebugBridge.hpp"


void EditorGuiComponent::renderUi()
{
    ImGui::Begin("Scene View");
    constexpr ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable;
    const auto tableVerticalSize = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10);

    const auto sceneManager = Prism::Utility::ServiceLocator::getService<Prism::Core::ISceneManager>();
    const auto actors = sceneManager->getActiveScene()->getActors();

    if (ImGui::BeginTable("Table_Actors", 1, flags, tableVerticalSize))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Actors", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();


        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(actors.size()));
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (static_cast<size_t>(row) < actors.size())
                {
                    const auto actorName = actors[row]->getName();
                    const bool previouslySelected = actors[row] == selectedActor;
                    const std::string actorNameWithId = std::format("{}##{}", actorName, row);
                    if (ImGui::Selectable(actorNameWithId.c_str(), previouslySelected,
                                          ImGuiSelectableFlags_SpanAllColumns))
                    {
                        if (actors[row] == selectedActor)
                        {
                            selectedActor = nullptr;
                        }
                        else
                        {
                            selectedActor = actors[row];
                        }
                    }
                }
            }
        }
        ImGui::EndTable();
    }
    
    //renderGraphicsDebugBridgeInfo();
    renderSelectedActorInfo(selectedActor);

    //ImGui::ShowDemoWindow();
    ImGui::End();
}

void EditorGuiComponent::renderSelectedActorInfo(const RawPtr<Prism::Core::Actor> actor)
{
    ImGui::Begin("Selected Actor");
    if (actor == nullptr)
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Name: %s", actor->getName().c_str());
    ImGui::SeparatorText("Transform");
    // Position
    {
        auto actorPosition = actor->getActorPosition();
        ImGui::Text("X: ");
        ImGui::SameLine();
        ImGui::InputFloat("##XPos", &actorPosition.x, 0.1f, 1.0f, "%.3f");
        ImGui::Text("Y: ");
        ImGui::SameLine();
        ImGui::InputFloat("##YPos", &actorPosition.y, 0.1f, 1.0f, "%.3f");
        ImGui::Text("Z: ");
        ImGui::SameLine();
        ImGui::InputFloat("##ZPos", &actorPosition.z, 0.1f, 1.0f, "%.3f");
        actor->setActorPosition(actorPosition);
    }
    // Rotation
    {
        auto actorRotation = actor->getActorRotation();
        ImGui::Text("X(Pitch): ");
        ImGui::SameLine();
        ImGui::InputFloat("##XRot", &actorRotation.x, 1.0f, 3.0f, "%.3f");
        ImGui::Text("Y(Yaw):   ");
        ImGui::SameLine();
        ImGui::InputFloat("##YRot", &actorRotation.y, 1.0f, 3.0f, "%.3f");
        ImGui::Text("Z(Roll):  ");
        ImGui::SameLine();
        ImGui::InputFloat("##ZRot", &actorRotation.z, 1.0f, 3.0f, "%.3f");
        actor->setActorRotation(actorRotation);
    }
    // Scale
    {
        auto actorScale = actor->getActorScale();
        ImGui::Text("X: ");
        ImGui::SameLine();
        ImGui::InputFloat("##XScale", &actorScale.x, 0.1f, 1.0f, "%.3f");
        ImGui::Text("Y: ");
        ImGui::SameLine();
        ImGui::InputFloat("##YScale", &actorScale.y, 0.1f, 1.0f, "%.3f");
        ImGui::Text("Z: ");
        ImGui::SameLine();
        ImGui::InputFloat("##ZScale", &actorScale.z, 0.1f, 1.0f, "%.3f");
        actor->setActorScale(actorScale);
    }

    if (const auto debugDrawHelper = dynamic_cast<Prism::Rendering::DebugDrawHelper*>(actor.get()))
    {
    }

    ImGui::End();
}

void EditorGuiComponent::renderGraphicsDebugBridgeInfo()
{
    ImGui::Begin("Graphics Debug Bridge");
    const auto graphicsDebugBridge = Prism::Utility::ServiceLocator::getService<
        Prism::Rendering::GraphicsDebugBridge>();

    ImGui::Text("Bridge Status: ");
    ImGui::SameLine();
    const auto color = graphicsDebugBridge->isInitialized()
                           ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
                           : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImGui::TextColored(color, "%s", graphicsDebugBridge->isInitialized() ? "Initialized" : "Uninitialized");

    ImGui::End();
}

void EditorGuiComponent::shutdown()
{
}
