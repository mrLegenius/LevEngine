#pragma once
#include "DataTypes/Path.h"

using String = eastl::string;

inline String ToString(const float value) { return std::to_string(value).c_str(); }
inline String ToString(const int value) { return std::to_string(value).c_str(); }
inline String ToString(const Path& value) { return value.string().c_str(); }

template <typename... T>
constexpr String Format(fmt::format_string<T...> format, T&&... args)
{
    return fmt::format(format, std::forward<T>(args)...).c_str();
}

template <>
struct std::hash<eastl::string>
{
    std::size_t operator()(const eastl::string& k) const noexcept
    {
        using std::size_t;
        using std::hash;
        using std::string;
        
        return eastl::hash<eastl::string>()(k);
    }
};
