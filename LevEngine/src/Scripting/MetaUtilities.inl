#pragma once

#include "MetaUtilities.h"

namespace LevEngine::Scripting
{
	template<class ...Args>
	inline auto InvokeMetaFunction(entt::meta_type metaType, entt::id_type functionId, Args &&...args)
	{
		if (!metaType)
		{
			Log::CoreWarning("Not valid entt::meta_type while InvokeMetaFunction");
			LEV_CORE_ASSERT(false, "Not valid entt::meta_type while InvokeMetaFunction")
		}
		else
		{
			if (auto metaFunction = metaType.func(functionId); metaFunction)
			{
				return metaFunction.invoke({}, eastl::forward<Args>(args)...);
			}
			Log::CoreWarning("Not valid function while InvokeMetaFunction");
		}

		return entt::meta_any{};
	}

	template <class... Args>
	inline auto InvokeMetaFunction(entt::id_type typeId, entt::id_type functionId, Args &&...args)
	{
		return InvokeMetaFunction(entt::resolve(typeId), functionId, eastl::forward<Args>(args)...);
	}
}
