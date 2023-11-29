#include "levpch.h"
#include "Entity.h"

#include "Components/Components.h"

namespace LevEngine
{
    Entity::Entity(const entt::handle handle) : m_Handle(handle) { }

    UUID Entity::GetUUID() const
    {
        return GetComponent<IDComponent>().ID;
    }

    String Entity::GetName() const
    {
        return GetComponent<TagComponent>().tag;
    }
}
