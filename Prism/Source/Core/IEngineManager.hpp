#pragma once
#include "../Utilities/IService.hpp"

namespace Prism::Core
{
    class IEngineManager : public Utility::IService
    {
    public:
        ~IEngineManager() override = default;

        [[nodiscard]] virtual bool isShutdownRequested() const = 0;
        virtual void requestShutdown() = 0;
    };
}
