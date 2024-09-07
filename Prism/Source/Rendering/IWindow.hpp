#pragma once
#include "../Core/ITickable.hpp"
#include <string>

namespace Prism::Rendering
{
    class IWindow : public Core::ITickable
    {
    public:
        virtual ~IWindow() override = default;
        virtual void init() = 0;
        virtual void tick(float deltaTime) override = 0;
        virtual void shutdown() = 0;
        virtual bool isShutdownRequested() = 0;
        virtual void pollWindowEvents() = 0;
        virtual void setWindowTitle(const std::string& title) = 0;
    };
}
