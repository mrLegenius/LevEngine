#include "levpch.h"
#include "MetaUtilities.h"


entt::id_type LevEngine::Scripting::GetIdType(const sol::table& component)
{
	if (!component.valid())
	{
		LEV_CORE_ASSERT(false, "Component has not been exposed to lua!");
		return -1;
	}

	const auto func = component["type_id"].get<sol::function>();
	LEV_CORE_ASSERT(func.valid(), "type_id function is not exposed to lua!");

	return func.valid() ? func().get<entt::id_type>() : -1;
}
