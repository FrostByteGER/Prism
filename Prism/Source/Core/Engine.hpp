#pragma once
#include "BaseBootstrapper.hpp"
#include "ISceneManager.hpp"
#include "../Utilities/EngineClock.hpp"
#include "../Utilities/Globals.hpp"

namespace Prism::Core
{
    class Engine
    {
    public:
        explicit Engine(BaseBootstrapper* bootstrapper);
        void setup(const std::vector<Utility::CommandLineArg>& commandLineArgs);
        void start();
        void shutdown();

    private:
        std::unique_ptr<BaseBootstrapper> bootstrapper;
        RawPtr<ISceneManager> sceneManager;
        Utility::EngineClock engineClock;

        float frameDeltaSeconds = 0;
        float framesPerSecond = 0;
        float frameAccumulatorSeconds = 0;
    };
}
