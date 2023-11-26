#pragma once

#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace LevEngine::Scripting
{

	[[nodiscard]] entt::id_type GetIdType(const sol::table& component);

	template<class ...Args>
	inline auto InvokeMetaFunction(entt::meta_type metaType, entt::id_type functionId, Args &&...args);

	template <class... Args>
	inline auto InvokeMetaFunction(entt::id_type typeId, entt::id_type functionId, Args &&...args);

	template <typename T> [[nodiscard]] entt::id_type DeduceType(T&& obj);
}

#include "MetaUtilities.inl"
