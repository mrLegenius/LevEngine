#pragma once
#include "EASTL/utility.h"

template<class TFirst, class TSecond>
using Pair = eastl::pair<TFirst, TSecond>;

template<class TFirst, class TSecond>
constexpr Pair<TFirst, TSecond> MakePair(const TFirst& first, const TSecond& second)
{
    return eastl::make_pair(first, second);
}