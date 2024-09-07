#pragma once
#include <string>

namespace Prism::Assets
{
    class Asset
    {
    public:
        explicit Asset(const std::string& name);

        [[nodiscard]] std::string getName() const
        {
            return name;
        }

        void setName(const std::string& name)
        {
            this->name = name;
        }

    private:
        std::string name;
    };
}
