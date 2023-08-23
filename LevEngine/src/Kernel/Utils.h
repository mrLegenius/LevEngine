#pragma once

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

template<typename Fn, typename Obj>
constexpr auto BindGetter(Fn fn, Obj obj)
{
	return std::bind(fn, obj);
}

template<typename Fn, typename Obj>
constexpr auto BindSetter(Fn fn, Obj obj)
{
	return std::bind(fn, obj, std::placeholders::_1);
}