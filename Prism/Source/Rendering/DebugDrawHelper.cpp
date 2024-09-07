#include "DebugDrawHelper.hpp"
#include "../Utilities/ServiceLocator.hpp"
#include "../Assets/AssetManager.hpp"

Prism::Rendering::DebugDrawHelper::DebugDrawHelper()
{
    setName("DebugDrawHelper");
}

void Prism::Rendering::DebugDrawHelper::init()
{
    Actor::init();
    const auto assetManager = Utility::ServiceLocator::getService<Assets::AssetManager>();
    sceneManager = Utility::ServiceLocator::getService<Core::ISceneManager>();
    lineMeshAsset = assetManager->getAsset<Assets::StaticMeshAsset>("Assets/StaticMeshes/Debug_Draw_Helper_Cube.obj");
}

void Prism::Rendering::DebugDrawHelper::shutdown()
{
    clearAll();
    Actor::shutdown();
}

void Prism::Rendering::DebugDrawHelper::tick(float deltaTime)
{
    Actor::tick(deltaTime);
    // Check all line and arrow meshes and destroy them if they are expired
    const auto currentTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).
        count();
    std::vector<RawPtr<Core::DebugLineActor>> lineActorsToRemove;
    for (const auto& lineActor : debugActors)
    {
        if (lineActor->getLifetime() > 0 && lineActor->getDeathTimeMilliseconds() >= 0 && currentTimeMilliseconds >=
            lineActor->getDeathTimeMilliseconds())
        {
            lineActorsToRemove.emplace_back(lineActor);
        }
    }

    for (const auto& toRemove : lineActorsToRemove)
    {
        std::erase_if(debugActors, [toRemove](const auto& a)
        {
            return a.get() == toRemove.get();
        });
    }

    for (const auto& lineActor : lineActorsToRemove)
    {
        sceneManager->unregisterActor(lineActor);
    }
}

void Prism::Rendering::DebugDrawHelper::drawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color,
                                                   const float thickness, const uint32_t lifetimeSec)
{
    const glm::vec3 midPoint = (from + to) / 2.0f;
    const auto direction = glm::normalize(midPoint);
    drawLine(from, direction, to.z, color, thickness, lifetimeSec);
}

void Prism::Rendering::DebugDrawHelper::drawLine(const glm::vec3& origin, const glm::vec3& direction,
                                                   const float length,
                                                   const glm::vec3& color, const float thickness,
                                                   const uint32_t lifetimeSec)
{
    const auto rotation = Core::Transform::lookAt(direction, Core::Transform::localUp);
    const auto lineActor = sceneManager->registerActor(std::make_unique<Core::DebugLineActor>());
    const auto staticMeshComp = lineActor->getFirstComponentOfType<Core::StaticMeshComponent>();
    lineActor->setName("DebugLine");
    staticMeshComp->setStaticMeshAsset(lineMeshAsset);
    staticMeshComp->setMeshColor(color);
    staticMeshComp->setCollidable(false);
    lineActor->setActorPosition(origin);
    lineActor->setActorScale(glm::vec3(thickness, thickness, length));
    lineActor->setActorRotation(rotation);
    lineActor->setLifetime(lifetimeSec * 1000);
    debugActors.emplace_back(lineActor);
}

void Prism::Rendering::DebugDrawHelper::drawArrow(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color,
                                                    const float thickness, const uint32_t lifetimeSec)
{
    const glm::vec3 midPoint = (from + to) / 2.0f;
    const auto direction = glm::normalize(midPoint);
    drawArrow(from, direction, to.z, color, thickness, lifetimeSec);
}

void Prism::Rendering::DebugDrawHelper::drawArrow(const glm::vec3& origin, const glm::vec3& direction,
                                                    const float length,
                                                    const glm::vec3& color, const float thickness,
                                                    const uint32_t lifetimeSec)
{
    // Main line
    const auto rotation = Core::Transform::lookAt(direction, Core::Transform::localUp);
    const auto mainLineActor = sceneManager->registerActor(std::make_unique<Core::DebugLineActor>());
    const auto mainLineStaticMeshComp = mainLineActor->getFirstComponentOfType<Core::StaticMeshComponent>();
    mainLineStaticMeshComp->setStaticMeshAsset(lineMeshAsset);
    mainLineStaticMeshComp->setMeshColor(color);
    mainLineStaticMeshComp->setCollidable(false);
    mainLineActor->setName("DebugArrow");
    mainLineActor->setActorPosition(origin);
    mainLineActor->setActorScale(glm::vec3(thickness, thickness, length));
    mainLineActor->setActorRotation(rotation);
    mainLineActor->setLifetime(lifetimeSec * 1000);
    debugActors.emplace_back(mainLineActor);

    const auto endPosition = origin + direction * length;

    const auto topLineActor = sceneManager->registerActor(std::make_unique<Core::DebugLineActor>());
    const auto bottomLineActor = sceneManager->registerActor(std::make_unique<Core::DebugLineActor>());
    const auto leftLineActor = sceneManager->registerActor(std::make_unique<Core::DebugLineActor>());
    const auto rightLineActor = sceneManager->registerActor(std::make_unique<Core::DebugLineActor>());

    topLineActor->setName("DebugArrowTop");
    bottomLineActor->setName("DebugArrowBottom");
    leftLineActor->setName("DebugArrowLeft");
    rightLineActor->setName("DebugArrowRight");

    const auto topLineStaticMeshComp = topLineActor->getFirstComponentOfType<Core::StaticMeshComponent>();
    topLineStaticMeshComp->setMeshColor(color);
    topLineStaticMeshComp->setCollidable(false);
    topLineStaticMeshComp->setStaticMeshAsset(lineMeshAsset);
    const auto bottomLineStaticMeshComp = bottomLineActor->getFirstComponentOfType<Core::StaticMeshComponent>();
    bottomLineStaticMeshComp->setMeshColor(color);
    bottomLineStaticMeshComp->setCollidable(false);
    bottomLineStaticMeshComp->setStaticMeshAsset(lineMeshAsset);
    const auto leftLineStaticMeshComp = leftLineActor->getFirstComponentOfType<Core::StaticMeshComponent>();
    leftLineStaticMeshComp->setMeshColor(color);
    leftLineStaticMeshComp->setCollidable(false);
    leftLineStaticMeshComp->setStaticMeshAsset(lineMeshAsset);
    const auto rightLineStaticMeshComp = rightLineActor->getFirstComponentOfType<Core::StaticMeshComponent>();
    rightLineStaticMeshComp->setMeshColor(color);
    rightLineStaticMeshComp->setCollidable(false);
    rightLineStaticMeshComp->setStaticMeshAsset(lineMeshAsset);

    topLineActor->setLifetime(lifetimeSec * 1000);
    bottomLineActor->setLifetime(lifetimeSec * 1000);
    leftLineActor->setLifetime(lifetimeSec * 1000);
    rightLineActor->setLifetime(lifetimeSec * 1000);
    debugActors.emplace_back(topLineActor);
    debugActors.emplace_back(bottomLineActor);
    debugActors.emplace_back(leftLineActor);
    debugActors.emplace_back(rightLineActor);

    topLineActor->setActorPosition(endPosition);
    bottomLineActor->setActorPosition(endPosition);
    leftLineActor->setActorPosition(endPosition);
    rightLineActor->setActorPosition(endPosition);

    topLineActor->setActorRotation(rotation);
    topLineActor->rotateActor(glm::vec3(arrowHeadAngleDegrees, 180.0f, 0.0f));
    bottomLineActor->setActorRotation(rotation);
    bottomLineActor->rotateActor(glm::vec3(-arrowHeadAngleDegrees, 180.0f, 0.0f));
    leftLineActor->setActorRotation(rotation);
    leftLineActor->rotateActor(glm::vec3(0.0f, -(180.0f + arrowHeadAngleDegrees), 0.0f));
    rightLineActor->setActorRotation(rotation);
    rightLineActor->rotateActor(glm::vec3(0.0f, 180.0f + arrowHeadAngleDegrees, 0.0f));

    topLineActor->setActorScale(glm::vec3(thickness, thickness, length * arrowHeadLength));
    bottomLineActor->setActorScale(glm::vec3(thickness, thickness, length * arrowHeadLength));
    leftLineActor->setActorScale(glm::vec3(thickness, thickness, length * arrowHeadLength));
    rightLineActor->setActorScale(glm::vec3(thickness, thickness, length * arrowHeadLength));
}

void Prism::Rendering::DebugDrawHelper::clearAll()
{
    for (const auto& lineActor : debugActors)
    {
        sceneManager->unregisterActor(lineActor);
    }
    debugActors.clear();
}
