#pragma once
#include "IEngineManager.hpp"

namespace Prism::Core
{
    class EngineManager : public IEngineManager
    {
    public:
        EngineManager() = default;
        ~EngineManager() override = default;

        void initialize() override
        {
        }

        void initializeDeferred() override
        {
        }

        void deInitialize() override
        {
        }

        [[nodiscard]] bool isShutdownRequested() const override
        {
            return shutdownRequested;
        }

        void requestShutdown() override
        {
            shutdownRequested = true;
        }

        std::string getFullName() override
        {
            return "Prism::Core::EngineManager";
        }

    private:
        bool shutdownRequested = false;
    };
}
