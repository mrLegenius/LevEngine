#pragma once

#define ENUM_AS_FLAG(X) \
inline X operator|(const X a, const X b) \
{ \
return static_cast<X>(static_cast<int>(a) | static_cast<int>(b)); \
} \
inline bool operator&(const X a, const X b) \
{ \
return static_cast<int>(a) & static_cast<int>(b); \
}