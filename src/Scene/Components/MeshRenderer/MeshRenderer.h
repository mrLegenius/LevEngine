#pragma once
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"

namespace LevEngine
{
	struct MeshRendererComponent
	{
		Ref<MeshAsset> mesh;
		Ref<MaterialAsset> material;
		bool castShadow = true;

		MeshRendererComponent() = default;
		MeshRendererComponent(const Ref<MeshAsset>& mesh) : mesh(mesh)
		{ }
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};

}
