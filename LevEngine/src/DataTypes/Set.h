#pragma once
#include "EASTL/set.h"

template<class Key>
using Set = eastl::set<Key>;

template<class Archive, class Key>
void save(Archive& output, Set<Key> const & set)
{
    output(set.size());
    for (auto vectorElement : set)
    {
        output(vectorElement);
    }
}

template<class Archive, class Key>
void load(Archive& input, Set<Key> & set)
{
    size_t size;
    input(size);

    for (size_t i = 0; i < size; ++i)
    {
        Key key;
        input(key);
        set.insert(key);
    }
} 