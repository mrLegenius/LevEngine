#include "pch.h"
#include "SkyboxAsset.h"

#include <imgui.h>

#include "TextureLibrary.h"
#include "yaml-cpp/node/node.h"

namespace LevEngine
{
	void SkyboxAsset::DrawProperties()
	{
		ImGui::Text("Left");
		ImGui::SameLine();
		GUIUtils::DrawTexture2D(
			[this] { return TextureLibrary::GetTexture(m_Left); }, 
			[this](const Ref<Texture>& texture) { SetTexture(Side::Left, texture->GetPath()); });

		ImGui::Text("Right");
		ImGui::SameLine();
		GUIUtils::DrawTexture2D(
			[this] { return TextureLibrary::GetTexture(m_Right); }, 
			[this](const Ref<Texture>& texture) { SetTexture(Side::Right, texture->GetPath()); });

		ImGui::Text("Bottom");
		ImGui::SameLine();
		GUIUtils::DrawTexture2D(
			[this] { return TextureLibrary::GetTexture(m_Bottom); },
			[this](const Ref<Texture>& texture) { SetTexture(Side::Bottom, texture->GetPath()); });

		ImGui::Text("Top");
		ImGui::SameLine();
		GUIUtils::DrawTexture2D(
			[this] { return TextureLibrary::GetTexture(m_Top); },
			[this](const Ref<Texture>& texture) { SetTexture(Side::Top, texture->GetPath()); });

		ImGui::Text("Back");
		ImGui::SameLine();
		GUIUtils::DrawTexture2D(
			[this] { return TextureLibrary::GetTexture(m_Back); }, 
			[this](const Ref<Texture>& texture) { SetTexture(Side::Back, texture->GetPath()); });

		ImGui::Text("Front");
		ImGui::SameLine();
		GUIUtils::DrawTexture2D(
			[this] { return TextureLibrary::GetTexture(m_Front); }, 
			[this](const Ref<Texture>& texture) { SetTexture(Side::Front, texture->GetPath()); });

		if (ImGui::Button("Save"))
			Asset::Serialize();
	}

	void SkyboxAsset::SetTexture(const Side side, std::string path)
	{
		switch (side)
		{
		case Side::Left:
			m_Left = std::move(path);
			break;
		case Side::Right:
			m_Right = std::move(path);
			break;
		case Side::Bottom:
			m_Bottom = std::move(path);
			break;
		case Side::Top:
			m_Top = std::move(path);
			break;
		case Side::Back:
			m_Back = std::move(path);
			break;
		case Side::Front:
			m_Front = std::move(path);
			break;
		default:
			LEV_THROW("Unknown Skybox Side")
		}

		InitTexture();
	}

	void SkyboxAsset::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Left" << YAML::Value << m_Left;
		out << YAML::Key << "Right" << YAML::Value << m_Right;
		out << YAML::Key << "Bottom" << YAML::Value << m_Bottom;
		out << YAML::Key << "Top" << YAML::Value << m_Top;
		out << YAML::Key << "Back" << YAML::Value << m_Back;
		out << YAML::Key << "Front" << YAML::Value << m_Front;
	}

	bool SkyboxAsset::Deserialize(YAML::Node& node)
	{
		m_Left = node["Left"].as<std::string>();
		m_Right = node["Right"].as<std::string>();
		m_Bottom = node["Bottom"].as<std::string>();
		m_Top = node["Top"].as<std::string>();
		m_Back = node["Back"].as<std::string>();
		m_Front = node["Front"].as<std::string>();

		InitTexture();

		return true;
	}

	void SkyboxAsset::InitTexture()
	{
		if (m_Left.empty()
		 || m_Right.empty()
		 || m_Bottom.empty()
		 || m_Top.empty()
		 || m_Back.empty()
		 || m_Front.empty())
			return;

		const std::string paths[6] = { m_Left, m_Right, m_Bottom, m_Top, m_Back, m_Front };
		m_Texture = Texture::CreateTextureCube(paths);
	}
}

