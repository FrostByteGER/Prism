#pragma once

#include "ITimeManager.hpp"
#include "../Utilities/EngineClock.hpp"

namespace Prism::Core
{
    class TimeManager : public ITimeManager
    {
    public:
        ~TimeManager() override = default;
        void initialize() override;
        void initializeDeferred() override;
        void deInitialize() override;

        std::string getFullName() override
        {
            return "Prism::Core::TimeManager";
        }


    private:
        Utility::EngineClock clock;
    };
}
