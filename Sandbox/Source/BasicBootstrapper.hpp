#pragma once
#include "Core/BaseBootstrapper.hpp"

class BasicBootstrapper : public Prism::Core::BaseBootstrapper
{
public:
    BasicBootstrapper() = default;

    explicit BasicBootstrapper(const RawPtr<Prism::Core::Scene> scenePtr)
        : BaseBootstrapper(scenePtr)
    {
    }

    void bootstrap(const std::vector<Prism::Utility::CommandLineArg>& commandLineArgs) override;
    void shutdown() override;
};
