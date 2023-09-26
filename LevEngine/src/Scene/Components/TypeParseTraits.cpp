#include "levpch.h"
#include "TypeParseTraits.h"

#include "Animation/WaypointMovement.h"
#include "Camera/Camera.h"
#include "Emitter/EmitterComponent.h"
#include "Lights/Lights.h"
#include "MeshRenderer/MeshRenderer.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/Rigidbody.h"
#include "SkyboxRenderer/SkyboxRenderer.h"
#include "Transform/Transform.h"

namespace LevEngine
{
    // Used to get component name by type. In order to get component, you need to register it below.
    // Registered component name can be retrieved using
    // TypeParseTraits<T>::name
    
    #define REGISTER_PARSE_TYPE(X) \
    template <> const char* TypeParseTraits<X>::name = #X

    // Register all engine components
    REGISTER_PARSE_TYPE(Transform);
    REGISTER_PARSE_TYPE(WaypointMovementComponent);
    REGISTER_PARSE_TYPE(CameraComponent);
    REGISTER_PARSE_TYPE(EmitterComponent);
    REGISTER_PARSE_TYPE(DirectionalLightComponent);
    REGISTER_PARSE_TYPE(PointLightComponent);
    REGISTER_PARSE_TYPE(MeshRendererComponent);
    REGISTER_PARSE_TYPE(SkyboxRendererComponent);
    REGISTER_PARSE_TYPE(BoxCollider);
    REGISTER_PARSE_TYPE(SphereCollider);
    REGISTER_PARSE_TYPE(Rigidbody);
}
