#pragma once
#include "EASTL/unordered_map.h"

template<class TKey, class TValue>
using UnorderedMap = eastl::unordered_map<TKey, TValue>;

template<class Archive, class TKey, class TValue>
void save(Archive& output, UnorderedMap<TKey, TValue> const & map)
{
    output(map.size());
    for (auto vectorElement : map)
    {
        output(vectorElement.first, vectorElement.second);
    }
}

template<class Archive, class TKey, class TValue>
void load(Archive& input, UnorderedMap<TKey, TValue> & map)
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