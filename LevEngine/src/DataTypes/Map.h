#pragma once
#include "EASTL/map.h"

template<class TKey, class TValue>
using Map = eastl::map<TKey, TValue>;

template<class Archive, class TKey, class TValue>
void save(Archive& output, Map<TKey, TValue> const & map)
{
    output(map.size());
    for (auto vectorElement : map)
    {
        output(vectorElement.first, vectorElement.second);
    }
}

template<class Archive, class TKey, class TValue>
void load(Archive& input, Map<TKey, TValue> & map)
{
    size_t size;
    input(size);

    for (size_t i = 0; i < size; ++i)
    {
        TKey key;
        TValue value;
        input(key);
        input(value);
        map.insert({key, value});
    }
} 