#pragma once

namespace Prism::Core
{
    class ITickable
    {
    public:
        virtual ~ITickable() = default;
        virtual void tick(float deltaTime) = 0;
    };
}
