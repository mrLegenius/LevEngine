#include "levpch.h"
#include "SkyboxAsset.h"
#include "Scene/Serializers/SerializerUtils.h"
#include "TextureAsset.h"

namespace LevEngine
{
	void SkyboxAsset::SerializeData(YAML::Emitter& out)
	{
		SerializeAsset(out, "Left", left);
		SerializeAsset(out, "Right", right);
		SerializeAsset(out, "Bottom", bottom);
		SerializeAsset(out, "Top", top);
		SerializeAsset(out, "Back", back);
		SerializeAsset(out, "Front", front);
	}

	void SkyboxAsset::DeserializeData(YAML::Node& node)
	{
		left = DeserializeAsset<TextureAsset>(node["Left"]);
		right = DeserializeAsset<TextureAsset>(node["Right"]);
		bottom = DeserializeAsset<TextureAsset>(node["Bottom"]);
		top = DeserializeAsset<TextureAsset>(node["Top"]);
		back = DeserializeAsset<TextureAsset>(node["Back"]);
		front = DeserializeAsset<TextureAsset>(node["Front"]);

		InitTexture();
	}

	void SkyboxAsset::InitTexture()
	{
		if (!left
		 || !right
		 || !bottom
		 || !top
		 || !back
		 || !front)
			return;

		const String paths[6] = {
			left->GetTexture()->GetPath(),
			right->GetTexture()->GetPath(),
			bottom->GetTexture()->GetPath(),
			top->GetTexture()->GetPath(),
			back->GetTexture()->GetPath(),
			front->GetTexture()->GetPath() };

		const auto linear = left->IsLinear
		|| right->IsLinear
		|| bottom->IsLinear
		|| top->IsLinear
		|| back->IsLinear
		|| front->IsLinear;

		m_Texture = Texture::CreateTextureCube(paths, linear);
	}
}

