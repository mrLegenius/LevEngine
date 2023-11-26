#include "levpch.h"
#include "Entity.h"

#include "Components/Components.h"

namespace LevEngine
{
    UUID Entity::GetUUID() const
    {
        return GetComponent<IDComponent>().ID;
    }

    String Entity::GetName() const
    {
        return GetComponent<TagComponent>().tag;
    }
}
