#pragma once
#include "../Utilities/IService.hpp"

namespace Prism::Core
{
    class ITimeManager : public Utility::IService
    {
    public:
        virtual ~ITimeManager() override = default;
        virtual void initialize() override = 0;
        virtual void initializeDeferred() override = 0;
        virtual void deInitialize() override = 0;

        virtual std::string getFullName() override = 0;
    };
}
