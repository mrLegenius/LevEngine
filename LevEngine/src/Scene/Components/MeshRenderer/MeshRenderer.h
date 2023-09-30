#pragma once
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(MeshRendererComponent);
	
	struct MeshRendererComponent
	{
		Ref<MeshAsset> mesh;
		Ref<MaterialAsset> material;
		bool castShadow = true;

		MeshRendererComponent();
		MeshRendererComponent(const Ref<MeshAsset>& mesh) : mesh(mesh)
		{ }
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};

}
