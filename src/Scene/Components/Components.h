#pragma once
#include "Kernel/UUID.h"

#include "OrbitCamera.h"

#include "Camera/Camera.h"
#include "Emitter/EmitterComponent.h"
#include "Lights/Lights.h"
#include "MeshRenderer/MeshRenderer.h"
#include "SkyboxRenderer/SkyboxRenderer.h"
#include "Transform/Transform.h"

#include "Physics/Components/Collider.h"
#include "Physics/Components/Rigidbody.h"

namespace LevEngine
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		explicit TagComponent(std::string other)
			: tag(std::move(other)) { }
	};

	template<typename... Component>
	struct ComponentGroup { };

	using AllComponents = ComponentGroup<
		Transform,
		Rigidbody,
		BoxCollider,
		SphereCollider,
		MeshRendererComponent,
		SkyboxRendererComponent,
		OrbitCamera,
		CameraComponent,
		DirectionalLightComponent,
		PointLightComponent,
		EmitterComponent>;
}
