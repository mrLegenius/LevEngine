#pragma once
#include "eastl/utility.h"

template <typename T>
constexpr typename eastl::remove_reference<T>::type&&
Move(T&& x) noexcept
{
    return eastl::move(x);
}