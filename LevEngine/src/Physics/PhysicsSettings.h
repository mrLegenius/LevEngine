#pragma once
#include "Physics/Components/FilterLayer.h"

namespace LevEngine
{
    class PhysicsSettings
    {
    public:
        [[nodiscard]] static bool IsCollisionEnabled(FilterLayer firstLayer, FilterLayer secondLayer);
        static void EnableCollision(FilterLayer firstLayer, FilterLayer secondLayer, bool flag);
    
    private:
        [[nodiscard]] static const FilterLayer& GetLayerCollisions(FilterLayer layer);
        
        [[nodiscard]] static int GetArrayIndex(FilterLayer layer);
        
        inline static Array<FilterLayer, 32> m_LayerCollisionArray {};
        
        friend class Physics;
        friend class Rigidbody;
        friend class CharacterController;
        friend class PhysicsSettingsSerializer;
    };
}