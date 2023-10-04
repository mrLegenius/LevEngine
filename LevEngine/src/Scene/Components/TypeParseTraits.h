#pragma once

namespace LevEngine
{
    #define REGISTER_PARSE_TYPE(X) \
    struct X; \
    template <> const char* TypeParseTraits<X>::name = #X
    
    template<typename T>
    struct TypeParseTraits
    {
        static const char* name;
    };
}
