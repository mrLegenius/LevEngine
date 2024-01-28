#include "levpch.h"
#include "PhysicsSettings.h"

namespace LevEngine
{
    const FilterLayer& PhysicsSettings::GetLayerCollisions(const FilterLayer layer)
    {
        if (static_cast<int>(layer) == static_cast<int>(FilterLayer::None)) return FilterLayer::None;
        if (static_cast<int>(layer) == static_cast<int>(FilterLayer::All)) return FilterLayer::All;
        
        return m_LayerCollisionArray.at(GetArrayIndex(layer));
    }

    int PhysicsSettings::GetArrayIndex(const FilterLayer layer)
    {
        return log2(static_cast<uint32_t>(layer));
    }
    
    bool PhysicsSettings::IsCollisionEnabled(const FilterLayer firstLayer, const FilterLayer secondLayer)
    {
        return GetLayerCollisions(firstLayer) & secondLayer;
    }

    void PhysicsSettings::EnableCollision(const FilterLayer firstLayer, const FilterLayer secondLayer, const bool flag)
    {
        const auto layerIndex = GetArrayIndex(firstLayer);
        
        if (flag)
        {
            m_LayerCollisionArray.at(layerIndex) = m_LayerCollisionArray.at(layerIndex) | secondLayer;
        }
        else
        {
            m_LayerCollisionArray.at(layerIndex) = static_cast<FilterLayer>(
                static_cast<uint32_t>(m_LayerCollisionArray.at(layerIndex)) & ~static_cast<uint32_t>(secondLayer)
            );
        }
    }
}