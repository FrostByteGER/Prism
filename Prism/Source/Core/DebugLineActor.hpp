#pragma once
#include "StaticMeshActor.hpp"

namespace Prism::Core
{
    class DebugLineActor : public StaticMeshActor
    {
    public:
        DebugLineActor() = default;
        ~DebugLineActor() override = default;
        void tick(float deltaTime) override;

        [[nodiscard]] int64_t getDeathTimeMilliseconds() const
        {
            return deathTimeMilliseconds;
        }

        [[nodiscard]] uint32_t getLifetime() const
        {
            return lifetimeMilliseconds;
        }

        void setLifetime(const uint32_t lifetimeMilliseconds)
        {
            this->lifetimeMilliseconds = lifetimeMilliseconds;
        }

    protected:
        void init() override;
        void initDeferred() override;
        void shutdown() override;

    private:
        int64_t deathTimeMilliseconds = -1;
        uint32_t lifetimeMilliseconds = 0;
    };
}
