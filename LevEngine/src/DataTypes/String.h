#pragma once
#include "EASTL/string.h"

using String = eastl::string;

template <typename T>
typename eastl::remove_reference<T>::type&&
Move(T&& x) noexcept
{
    return eastl::move(x);
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
