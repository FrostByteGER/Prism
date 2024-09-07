#pragma once
#include "Core/Scene.hpp"

class SandboxScene : public Prism::Core::Scene
{
public:
    SandboxScene();
    ~SandboxScene() override;
    void tick(float deltaTime) override;

protected:
    void init() override;
    void beginPlay() override;
    void shutdown() override;
    void initGuiComponents() override;
};
