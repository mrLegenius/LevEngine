#include "levpch.h"
#include "Entity.h"
namespace LevEngine
{
template <typename T>
void Entity::AddScript()
{
    LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid!");
    LEV_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component!");
}
}