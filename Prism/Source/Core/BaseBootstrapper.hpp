#pragma once
#include "Scene.hpp"
#include "../Utilities/Globals.hpp"
#include "../Utilities/CommandLineArgsManager.hpp"

namespace Prism::Core
{
    class BaseBootstrapper
    {
    public:
        BaseBootstrapper() = default;

        explicit BaseBootstrapper(const RawPtr<Scene> scenePtr) : defaultScene(scenePtr)
        {
        }

        virtual ~BaseBootstrapper() = default;
        void bootstrapInternal(const std::vector<Utility::CommandLineArg>& commandLineArgs);
        virtual void bootstrap(const std::vector<Utility::CommandLineArg>& commandLineArgs) = 0;
        void shutdownInternal();
        virtual void shutdown() = 0;

        [[nodiscard]] RawPtr<Scene> getDefaultScene() const
        {
            return defaultScene;
        }

    protected:
        RawPtr<Scene> defaultScene = nullptr;
    };
}
