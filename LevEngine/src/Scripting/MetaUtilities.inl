#pragma once

#include "MetaUtilities.h"

namespace LevEngine::Scripting
{
	template<class ...Args>
	inline auto InvokeMetaFunction(entt::meta_type metaType, entt::id_type functionId, Args &&...args)
	{
		if (!metaType)
		{
			LEV_CORE_ASSERT(false, "Not valid entt::meta_type while InvokeMetaFunction")
		}
		
		if (auto metaFunction = metaType.func(functionId); metaFunction)
		{
			return metaFunction.invoke({}, eastl::forward<Args>(args)...);
		}
		Log::CoreWarning("Not valid function while InvokeMetaFunction");

		return entt::meta_any{};
	}

	template <class... Args>
	inline auto InvokeMetaFunction(entt::id_type typeId, entt::id_type functionId, Args &&...args)
	{
		auto metaType = entt::resolve(typeId);
		return InvokeMetaFunction(metaType, functionId, eastl::forward<Args>(args)...);
	}

	template <typename T>
	entt::id_type DeduceType(T&& obj)
	{
		switch (obj.get_type()) {
		// in m_Lua: registry:has(e, Transform.type_id())
		case sol::type::number:
			return obj.as<entt::id_type>();
		// in m_Lua: registry:has(e, Transform)
		case sol::type::table:
			return get_type_id(obj);
		}
		LEV_THROW("Wrong sol::type")
	}
}
