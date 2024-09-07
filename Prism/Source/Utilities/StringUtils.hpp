#pragma once
#include <sstream>
#include <string>
#include <vector>

namespace Prism::Utility
{
    class StringUtils
    {
    public:
        [[nodiscard]] static std::vector<std::string> split(const std::string& str, const char delimiter)
        {
            std::vector<std::string> tokens;
            std::stringstream ss(str);
            std::string token;

            while (std::getline(ss, token, delimiter))
            {
                tokens.push_back(token);
            }

            return tokens;
        }

        [[nodiscard]] static std::string trim(const std::string& str)
        {
            auto start = str.begin();
            while (start != str.end() && std::isspace(*start))
            {
                ++start;
            }

            auto end = str.end();
            do
            {
                --end;
            }
            while (std::distance(start, end) > 0 && std::isspace(*end));

            return {start, end + 1};
        }
    };

    [[nodiscard]] static bool equalsIgnoreCase(const std::string& str1, const std::string& str2)
    {
        return std::ranges::equal(str1, str2, [](const char c1, const char c2)
        {
            return std::tolower(c1) == std::tolower(c2);
        });
    }
}
