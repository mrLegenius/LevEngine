#pragma once
#include "EASTL/shared_ptr.h"
#include "EASTL/unique_ptr.h"

#pragma region Scope
namespace LevEngine
{
template<typename T>
using Scope = eastl::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return eastl::make_unique<T>(eastl::forward<Args>(args)...);
}

template<typename T>
constexpr Scope<T> CreateScope(T object)
{
	return eastl::make_unique<T>(eastl::move(object));
}

template<typename T, typename List>
constexpr Scope<T> CreateScope(std::initializer_list<List> list)
{
	return Scope<T>{ new T(list) };
}
#pragma endregion

#pragma region Ref

template<typename T>
using Ref = eastl::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return eastl::make_shared<T>(eastl::forward<Args>(args)...);
}

template<typename T>
constexpr Ref<T> CreateRef(T object)
{
	return eastl::make_shared<T>(eastl::move(object));
}

template<typename T, typename List>
constexpr Ref<T> CreateRef(std::initializer_list<List> list)
{
	return Ref<T>{ new T(list) };
}

template<class TDerived, class TBase>
constexpr Ref<TDerived> CastRef(Ref<TBase> ptr)
{
	static_assert(eastl::is_base_of_v<TBase, TDerived>, "TDerived must be derived from TBase");
	return eastl::dynamic_pointer_cast<TDerived>(ptr);
}
#pragma endregion

#pragma region Weak

	template<typename T>
	using Weak = eastl::weak_ptr<T>;
	
#pragma endregion
}