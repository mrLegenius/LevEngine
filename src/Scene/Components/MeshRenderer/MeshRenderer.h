#pragma once
#include "Assets/MaterialAsset.h"
#include "Renderer/3D/Mesh.h"

namespace LevEngine
{
	struct MeshRendererComponent
	{
		Ref<Mesh> mesh;
		Ref<MaterialAsset> material;
		bool castShadow = true;

		MeshRendererComponent() = default;
		MeshRendererComponent(const Ref<Mesh>& mesh) : mesh(mesh)
		{ }
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};

}
