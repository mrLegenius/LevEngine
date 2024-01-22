#pragma once
#include "EASTL/vector.h"

template<class T>
using Vector = eastl::vector<T>;

template<class Archive, class T>
void save(Archive& output, Vector<T> const & vector)
{
    output(vector.size());
    for (auto vectorElement : vector)
    {
        output(vectorElement);
    }
}

template<class Archive, class T>
void load(Archive& input, Vector<T> & vector)
{
    size_t size;
    input(size);

    vector.resize(size);
    for (int i = 0; i < size; ++i)
    {
        input(vector[i]);
    }
} 