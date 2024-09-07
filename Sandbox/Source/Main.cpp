#ifdef _WIN64
#include <Windows.h>
#else
#include<iostream>
#endif
#include "Core/Engine.hpp"
#include "BasicBootstrapper.hpp"
#include "Utilities/Logging/Log.hpp"
#include "SandboxScene.hpp"
#include "cxxopts.hpp"
#include "Utilities/CommandLineArgsManager.hpp"

int main(int argc, char* argv[])
{
    LOG_DEBUG("Welcome to Prism!");
    try
    {
        cxxopts::Options options("Prism", "Prism");
        options.add_options()
            ("d,debug", "Enable debug logging", cxxopts::value<bool>()->default_value("false"))
            ("h,help", "Print usage")
            ("v,version", "Print version")
            ("headless", "(NOT WORKING YET!) Disable window rendering", cxxopts::value<bool>()->default_value("false"));


        auto parsedOptions = options.parse(argc, argv);

        std::vector<Prism::Utility::CommandLineArg> commandLineArgs;
        for (const auto& kvp : parsedOptions.arguments())
        {
            commandLineArgs.push_back({kvp.key(), kvp.value()});
        }

        auto* scene = new SandboxScene();
        auto engine = Prism::Core::Engine(new BasicBootstrapper(scene));
        engine.setup(commandLineArgs);
        engine.start();
        engine.shutdown();
    }
    catch (...)
    {
        const std::exception_ptr ePtr = std::current_exception();
        try
        {
            if (ePtr)
            {
                std::rethrow_exception(ePtr);
            }
            else
            {
                LOG_CRITICAL("An exception was caught at top-level but the pointer to it is null!");
            }
        }
        catch (const std::exception& e)
        {
            LOG_CRITICAL("Caught exception at top-level: \"" + std::string(e.what()) + "\"");
        }
    }
#if !defined(SKIP_INPUT_PROMPT) 
#ifdef _WIN64
    system("pause");
#else
    std::cout << "Press ENTER to continue..." << std::endl;
    std::cin.get();
#endif
    return 0;
#endif
}
