#pragma once
#include <glm/vec3.hpp>
#include "../Core/ISceneManager.hpp"
#include "../Core/DebugLineActor.hpp"
#include "../Assets/StaticMeshAsset.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Rendering
{
    class DebugDrawHelper : public Core::Actor
    {
    public:
        DebugDrawHelper();
        ~DebugDrawHelper() override = default;
        void tick(float deltaTime) override;
        void drawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float thickness = 1.0f,
                      uint32_t lifetimeSec = 5.0f);
        void drawLine(const glm::vec3& origin, const glm::vec3& direction, float length, const glm::vec3& color,
                      float thickness = 1.0f, uint32_t lifetimeSec = 5.0f);
        void drawArrow(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float thickness = 1.0f,
                       uint32_t lifetimeSec = 5.0f);
        void drawArrow(const glm::vec3& origin, const glm::vec3& direction, float length, const glm::vec3& color,
                       float thickness = 1.0f, uint32_t lifetimeSec = 5.0f);
        void clearAll();

    protected:
        void init() override;
        void shutdown() override;

    private:
        RawPtr<Core::ISceneManager> sceneManager;
        RawPtr<Assets::StaticMeshAsset> lineMeshAsset;
        std::vector<RawPtr<Core::DebugLineActor>> debugActors;

        inline constexpr static float arrowHeadAngleDegrees = 30.0f;
        inline constexpr static float arrowHeadLength = 0.125f;
    };
}
