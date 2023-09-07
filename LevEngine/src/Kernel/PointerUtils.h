#pragma once

#pragma region Scope
namespace LevEngine
{
template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
constexpr Scope<T> CreateScope(T object)
{
	return std::make_unique<T>(std::move(object));
}

template<typename T, typename List>
constexpr Scope<T> CreateScope(std::initializer_list<List> list)
{
	return Scope<T>{ new T(list) };
}
#pragma endregion

#pragma region Ref

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
constexpr Ref<T> CreateRef(T object)
{
	return std::make_shared<T>(std::move(object));
}

template<typename T, typename List>
constexpr Ref<T> CreateRef(std::initializer_list<List> list)
{
	return Ref<T>{ new T(list) };
}

template<class TDerived, class TBase>
constexpr Ref<TDerived> CastRef(Ref<TBase> ptr)
{
	static_assert(std::is_base_of_v<TBase, TDerived>, "TDerived must be derived from TBase");
	return std::dynamic_pointer_cast<TDerived>(ptr);
}
#pragma endregion

#pragma region Weak

	template<typename T>
	using Weak = std::weak_ptr<T>;
	
#pragma endregion
}