#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	class MaterialAsset;
	class MeshAsset;
	REGISTER_PARSE_TYPE(MeshRendererComponent);
	
	struct MeshRendererComponent
	{
		bool enabled = true;
		Ref<MeshAsset> mesh;
		Ref<MaterialAsset> material;
		bool castShadow = true;

		MeshRendererComponent();
		MeshRendererComponent(const Ref<MeshAsset>& mesh) : mesh(mesh)
		{ }
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};

}
