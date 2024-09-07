#pragma once
#include <string>

namespace Prism::Utility
{
    class IService
    {
    public:
        virtual ~IService() = default;
        virtual void initialize() = 0;
        virtual void initializeDeferred() = 0;
        virtual void deInitialize() = 0;
        virtual std::string getFullName() = 0;
    };
}
