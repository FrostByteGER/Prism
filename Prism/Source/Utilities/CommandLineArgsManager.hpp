#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include "IService.hpp"

namespace Prism::Utility
{
    struct CommandLineArg
    {
        std::string name;
        std::string rawValue;
    };

    class CommandLineArgsManager : public IService
    {
    public:
        explicit CommandLineArgsManager(std::vector<CommandLineArg> commandLineArgs)
            : commandLineArgs(std::move(commandLineArgs))
        {
        }

        void initialize() override
        {
        }

        void initializeDeferred() override
        {
        }

        void deInitialize() override
        {
        }

        std::string getFullName() override
        {
            return "Prism::Utilities::CommandLineArgsManager";
        }

        [[nodiscard]] bool hasArg(const std::string& argName) const;
        [[nodiscard]] bool hasArgValue(const std::string& argName, const std::string& argValue) const;
        [[nodiscard]] bool hasArgCharValue(const std::string& argName, char argValue) const;
        [[nodiscard]] bool hasArgFloatValue(const std::string& argName, float argValue) const;
        [[nodiscard]] bool hasArgDoubleValue(const std::string& argName, double argValue) const;
        [[nodiscard]] bool hasArgInt32Value(const std::string& argName, int32_t argValue) const;
        [[nodiscard]] bool hasArgInt64Value(const std::string& argName, int64_t argValue) const;
        [[nodiscard]] bool hasArgUInt32Value(const std::string& argName, uint32_t argValue) const;
        [[nodiscard]] bool hasArgUInt64Value(const std::string& argName, uint64_t argValue) const;
        [[nodiscard]] bool hasArgBoolValue(const std::string& argName, bool argValue) const;

        [[nodiscard]] std::string getArgValue(const std::string& argName) const;
        [[nodiscard]] std::string getArgValue(const std::string& argName, const std::string& defaultValue) const;
        [[nodiscard]] char getArgAsChar(const std::string& argName) const;
        [[nodiscard]] char getArgAsChar(const std::string& argName, char defaultValue) const;
        [[nodiscard]] float getArgValueAsFloat(const std::string& argName) const;
        [[nodiscard]] float getArgValueAsFloat(const std::string& argName, float defaultValue) const;
        [[nodiscard]] double getArgValueAsDouble(const std::string& argName) const;
        [[nodiscard]] double getArgValueAsDouble(const std::string& argName, double defaultValue) const;
        [[nodiscard]] int32_t getArgValueAsInt32(const std::string& argName) const;
        [[nodiscard]] int32_t getArgValueAsInt32(const std::string& argName, int32_t defaultValue) const;
        [[nodiscard]] int64_t getArgValueAsInt64(const std::string& argName) const;
        [[nodiscard]] int64_t getArgValueAsInt64(const std::string& argName, int64_t defaultValue) const;
        [[nodiscard]] uint32_t getArgValueAsUInt32(const std::string& argName) const;
        [[nodiscard]] uint32_t getArgValueAsUInt32(const std::string& argName, uint32_t defaultValue) const;
        [[nodiscard]] uint64_t getArgValueAsUInt64(const std::string& argName) const;
        [[nodiscard]] uint64_t getArgValueAsUInt64(const std::string& argName, uint64_t defaultValue) const;
        [[nodiscard]] bool getArgValueAsBool(const std::string& argName) const;
        [[nodiscard]] bool getArgValueAsBool(const std::string& argName, bool defaultValue) const;

        [[nodiscard]] std::vector<CommandLineArg> getAllArgs() const;

    private:
        std::vector<CommandLineArg> commandLineArgs;
    };
}
