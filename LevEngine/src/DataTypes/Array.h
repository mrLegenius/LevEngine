#pragma once
#include "EASTL/array.h"

template <typename T, size_t N = 1>
using Array = eastl::array<T, N>;

template<class Archive, typename T, size_t N>
void save(Archive& output, Array<T, N> const & array)
{
    for (auto arrayElement : array)
    {
        output(arrayElement);
    }
}

template<class Archive, typename T, size_t N>
void load(Archive& input, Array<T, N> & array)
{
    for (size_t i = 0; i < N; ++i)
    {
        input(array[i]);
    }
} 