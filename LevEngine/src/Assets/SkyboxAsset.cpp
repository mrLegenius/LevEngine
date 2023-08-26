#include "levpch.h"
#include "SkyboxAsset.h"
#include "GUI/GUIUtils.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	void SkyboxAsset::DrawProperties()
	{
		GUIUtils::DrawTextureAsset("Left", &m_Left);
		GUIUtils::DrawTextureAsset("Right", &m_Right);
		GUIUtils::DrawTextureAsset("Bottom", &m_Bottom);
		GUIUtils::DrawTextureAsset("Top", &m_Top);
		GUIUtils::DrawTextureAsset("Back", &m_Back);
		GUIUtils::DrawTextureAsset("Front", &m_Front);
	}

	void SkyboxAsset::SerializeData(YAML::Emitter& out)
	{
		SerializeAsset(out, "Left", m_Left);
		SerializeAsset(out, "Right", m_Right);
		SerializeAsset(out, "Bottom", m_Bottom);
		SerializeAsset(out, "Top", m_Top);
		SerializeAsset(out, "Back", m_Back);
		SerializeAsset(out, "Front", m_Front);
	}

	void SkyboxAsset::DeserializeData(YAML::Node& node)
	{
		DeserializeAsset<TextureAsset>(node["Left"]);

		m_Left = DeserializeAsset<TextureAsset>(node["Left"]);
		m_Right = DeserializeAsset<TextureAsset>(node["Right"]);
		m_Bottom = DeserializeAsset<TextureAsset>(node["Bottom"]);
		m_Top = DeserializeAsset<TextureAsset>(node["Top"]);
		m_Back = DeserializeAsset<TextureAsset>(node["Back"]);
		m_Front = DeserializeAsset<TextureAsset>(node["Front"]);

		InitTexture();
	}

	void SkyboxAsset::InitTexture()
	{
		if (!m_Left
		 || !m_Right
		 || !m_Bottom
		 || !m_Top
		 || !m_Back
		 || !m_Front)
			return;

		const std::string paths[6] = {
			m_Left->GetTexture()->GetPath(),
			m_Right->GetTexture()->GetPath(),
			m_Bottom->GetTexture()->GetPath(),
			m_Top->GetTexture()->GetPath(),
			m_Back->GetTexture()->GetPath(),
			m_Front->GetTexture()->GetPath() };

		m_Texture = Texture::CreateTextureCube(paths);
	}
}

