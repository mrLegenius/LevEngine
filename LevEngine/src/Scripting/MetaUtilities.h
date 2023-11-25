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

	template <typename T> [[nodiscard]] entt::id_type deduce_type(T&& obj) {
		switch (obj.get_type()) {
			// in m_Lua: registry:has(e, Transform.type_id())
		case sol::type::number:
			return obj.as<entt::id_type>();
			// in m_Lua: registry:has(e, Transform)
		case sol::type::table:
			return get_type_id(obj);
		}
		assert(false);
		return -1;
	}
}

#include "MetaUtilities.inl"
