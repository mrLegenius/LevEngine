#include "Entity.h"
#include "Components.h"

namespace LevEngine
{
	Entity::Entity(const entt::entity id, Scene* scene)
		: m_EntityID(id), m_Scene(scene) { }

    template <typename T>
    void Entity::AddScript()
    {
        LEV_CORE_ASSERT(m_Scene->m_Registry.valid(m_EntityID), "Entity is not valid!");
        LEV_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component!");

        return AddComponent<NativeScriptComponent>().Bind<T>();
    }
}
