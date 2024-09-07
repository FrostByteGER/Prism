#include "CommandLineArgsManager.hpp"

#include <algorithm>
#include "Globals.hpp"

bool Prism::Utility::CommandLineArgsManager::hasArg(const std::string& argName) const
{
    return std::ranges::any_of(commandLineArgs, [&argName](const CommandLineArg& arg) { return arg.name == argName; });
}

bool Prism::Utility::CommandLineArgsManager::hasArgValue(const std::string& argName,
                                                           const std::string& argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        return arg.name == argName && arg.rawValue == argValue;
    });
}

bool Prism::Utility::CommandLineArgsManager::hasArgCharValue(const std::string& argName,
                                                               char argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        return arg.name == argName && arg.rawValue == std::string(1, argValue);
    });
}

bool Prism::Utility::CommandLineArgsManager::hasArgFloatValue(const std::string& argName, float argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        try
        {
            const auto val = std::stof(arg.rawValue);
            return arg.name == argName && compareFloats(val, argValue);
        }
        catch (const std::exception&)
        {
            return false;
        }
    });
}

bool Prism::Utility::CommandLineArgsManager::hasArgDoubleValue(const std::string& argName, double argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        try
        {
            const auto val = std::stod(arg.rawValue);
            return arg.name == argName && compareDoubles(val, argValue);
        }
        catch (const std::exception&)
        {
            return false;
        }
    });
}

bool Prism::Utility::CommandLineArgsManager::hasArgInt32Value(const std::string& argName, int32_t argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        try
        {
            const auto val = std::stoi(arg.rawValue);
            return arg.name == argName && val == argValue;
        }
        catch (const std::exception&)
        {
            return false;
        }
    });
}

bool Prism::Utility::CommandLineArgsManager::hasArgInt64Value(const std::string& argName, int64_t argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        try
        {
            const auto val = std::stoll(arg.rawValue);
            return arg.name == argName && val == argValue;
        }
        catch (const std::exception&)
        {
            return false;
        }
    });
}

bool Prism::Utility::CommandLineArgsManager::hasArgUInt32Value(const std::string& argName, uint32_t argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        try
        {
            const auto val = std::stoul(arg.rawValue);
            return arg.name == argName && val == argValue;
        }
        catch (const std::exception&)
        {
            return false;
        }
    });
}

bool Prism::Utility::CommandLineArgsManager::hasArgUInt64Value(const std::string& argName, uint64_t argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        try
        {
            const auto val = std::stoull(arg.rawValue);
            return arg.name == argName && val == argValue;
        }
        catch (const std::exception&)
        {
            return false;
        }
    });
}

bool Prism::Utility::CommandLineArgsManager::hasArgBoolValue(const std::string& argName, bool argValue) const
{
    return std::ranges::any_of(commandLineArgs, [&argName, &argValue](const CommandLineArg& arg)
    {
        return arg.name == argName && (arg.rawValue == (argValue ? "true" : "false") || arg.rawValue == (
            argValue ? "1" : "0"));
    });
}

std::string Prism::Utility::CommandLineArgsManager::getArgValue(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValue: Argument not found: " + argName);
    }
    return it->rawValue;
}

std::string Prism::Utility::CommandLineArgsManager::getArgValue(const std::string& argName,
                                                                  const std::string& defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    return it != commandLineArgs.end() ? it->rawValue : defaultValue;
}

char Prism::Utility::CommandLineArgsManager::getArgAsChar(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgAsChar: Argument not found: " + argName);
    }
    return it->rawValue[0];
}

char Prism::Utility::CommandLineArgsManager::getArgAsChar(const std::string& argName,
                                                            const char defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    return it != commandLineArgs.end() ? it->rawValue[0] : defaultValue;
}

float Prism::Utility::CommandLineArgsManager::getArgValueAsFloat(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsFloat: Argument not found: " + argName);
    }
    try
    {
        return std::stof(it->rawValue);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsFloat: Argument value not a float: " + argName);
    }
}

float Prism::Utility::CommandLineArgsManager::getArgValueAsFloat(const std::string& argName,
                                                                   const float defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });

    if (it != commandLineArgs.end())
    {
        try
        {
            return std::stof(it->rawValue);
        }
        catch (const std::exception&)
        {
            throw std::runtime_error(
                "CommandLineArgsManager::getArgValueAsFloat: Argument value not a float: " + argName);
        }
    }

    return defaultValue;
}

double Prism::Utility::CommandLineArgsManager::getArgValueAsDouble(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsDouble: Argument not found: " + argName);
    }

    try
    {
        return std::stod(it->rawValue);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error(
            "CommandLineArgsManager::getArgValueAsDouble: Argument value not a double: " + argName);
    }
}

double Prism::Utility::CommandLineArgsManager::getArgValueAsDouble(const std::string& argName,
                                                                     const double defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });

    if (it != commandLineArgs.end())
    {
        try
        {
            return std::stod(it->rawValue);
        }
        catch (const std::exception&)
        {
            throw std::runtime_error(
                "CommandLineArgsManager::getArgValueAsDouble: Argument value not a double: " + argName);
        }
    }

    return defaultValue;
}

int32_t Prism::Utility::CommandLineArgsManager::getArgValueAsInt32(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsInt32: Argument not found: " + argName);
    }

    try
    {
        return std::stoi(it->rawValue);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsInt32: Argument value not an int32_t: " +
            argName);
    }
}

int32_t Prism::Utility::CommandLineArgsManager::getArgValueAsInt32(const std::string& argName,
                                                                     const int32_t defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });

    if (it != commandLineArgs.end())
    {
        try
        {
            return std::stoi(it->rawValue);
        }
        catch (const std::exception&)
        {
            throw std::runtime_error("CommandLineArgsManager::getArgValueAsInt32: Argument value not a int32_t: " +
                argName);
        }
    }

    return defaultValue;
}

int64_t Prism::Utility::CommandLineArgsManager::getArgValueAsInt64(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsInt64: Argument not found: " + argName);
    }

    try
    {
        return std::stoll(it->rawValue);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsInt64: Argument value not an int64_t: " +
            argName);
    }
}

int64_t Prism::Utility::CommandLineArgsManager::getArgValueAsInt64(const std::string& argName,
                                                                     const int64_t defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });

    if (it != commandLineArgs.end())
    {
        try
        {
            return std::stoll(it->rawValue);
        }
        catch (const std::exception&)
        {
            throw std::runtime_error("CommandLineArgsManager::getArgValueAsInt64: Argument value not a int64_t: " +
                argName);
        }
    }

    return defaultValue;
}

uint32_t Prism::Utility::CommandLineArgsManager::getArgValueAsUInt32(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsUInt32: Argument not found: " + argName);
    }

    try
    {
        return std::stoul(it->rawValue);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsUInt32: Argument value not a uint32_t: " +
            argName);
    }
}

uint32_t Prism::Utility::CommandLineArgsManager::getArgValueAsUInt32(const std::string& argName,
                                                                       const uint32_t defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });

    if (it != commandLineArgs.end())
    {
        try
        {
            return std::stoul(it->rawValue);
        }
        catch (const std::exception&)
        {
            throw std::runtime_error("CommandLineArgsManager::getArgValueAsUInt32: Argument value not a uint32_t: " +
                argName);
        }
    }

    return defaultValue;
}

uint64_t Prism::Utility::CommandLineArgsManager::getArgValueAsUInt64(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsUInt64: Argument not found: " + argName);
    }

    try
    {
        return std::stoull(it->rawValue);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsUInt64: Argument value not a uint64_t: " +
            argName);
    }
}

uint64_t Prism::Utility::CommandLineArgsManager::getArgValueAsUInt64(const std::string& argName,
                                                                       const uint64_t defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });

    if (it != commandLineArgs.end())
    {
        try
        {
            return std::stoull(it->rawValue);
        }
        catch (const std::exception&)
        {
            throw std::runtime_error("CommandLineArgsManager::getArgValueAsUInt64: Argument value not a uint64_t: " +
                argName);
        }
    }

    return defaultValue;
}

bool Prism::Utility::CommandLineArgsManager::getArgValueAsBool(const std::string& argName) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    if (it == commandLineArgs.end())
    {
        throw std::runtime_error("CommandLineArgsManager::getArgValueAsBool: Argument not found: " + argName);
    }
    return it->rawValue == "true" || it->rawValue == "1";
}

bool Prism::Utility::CommandLineArgsManager::getArgValueAsBool(const std::string& argName,
                                                                 const bool defaultValue) const
{
    const auto it = std::ranges::find_if(commandLineArgs, [&argName](const CommandLineArg& arg)
    {
        return arg.name == argName;
    });
    return it != commandLineArgs.end()
               ? it->rawValue == "true" || it->rawValue == "1"
               : defaultValue;
}

std::vector<Prism::Utility::CommandLineArg> Prism::Utility::CommandLineArgsManager::getAllArgs() const
{
    return commandLineArgs;
}
