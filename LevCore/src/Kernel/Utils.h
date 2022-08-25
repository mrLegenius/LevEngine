#pragma once

#define LEV_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

constexpr int Bit(const int offset) { return 1 << offset; }