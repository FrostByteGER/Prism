#pragma once
#include "object_ptr.hpp"
#include "Logging/Log.hpp"

template <typename T>
using RawPtr = jss::object_ptr<T>;

namespace Prism
{
    [[nodiscard]] inline bool compareFloats(const float f1, const float f2)
    {
        static constexpr float epsilon = 1.0e-05f;
        if (std::abs(f1 - f2) <= epsilon)
            return true;
        return std::abs(f1 - f2) <= epsilon * std::max(std::abs(f1), std::abs(f2));
    }

    [[nodiscard]] inline bool compareDoubles(const double f1, const double f2)
    {
        static constexpr double epsilon = 1.0e-05f;
        if (std::abs(f1 - f2) <= epsilon)
            return true;
        return std::abs(f1 - f2) <= epsilon * std::max(std::abs(f1), std::abs(f2));
    }
}
