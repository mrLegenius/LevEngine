#pragma once
#include "EASTL/unordered_map.h"

template<class TKey, class TValue>
using UnorderedMap = eastl::unordered_map<TKey, TValue>;
