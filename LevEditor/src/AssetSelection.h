#pragma once
#include "Assets/Asset.h"
#include "Selection.h"
#include "GUI/Drawers/Assets/AssetDrawer.h"

namespace LevEngine
{
	class AssetSelection final : public Selection
	{
	public:
		AssetSelection() = default;
		explicit AssetSelection(const Ref<Asset>& asset) : m_Asset(asset) { }

		void Set(const Ref<Asset>& asset) { m_Asset = asset; }
		[[nodiscard]] const Ref<Asset>& Get() const { return m_Asset; }

		void DrawProperties() override
		{
			if (!m_Asset) return;

			auto name = m_Asset->GetName();

			char buffer[256] = {};
			strcpy_s(buffer, sizeof buffer, name.c_str());

			ImGui::AlignTextToFramePadding();

			if (ImGui::InputText("##Name", buffer, sizeof buffer, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				AssetDatabase::RenameAsset(m_Asset, buffer);
			}

			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 64);
			if (ImGui::Button("Save", ImVec2{ 64, 24 }))
				m_Asset->Serialize();

			ImGui::Separator();

			static auto& assetDrawers = OrderedClassCollection<Editor::IAssetDrawer>::Instance();
			
			for (const auto& assetDrawer : assetDrawers)
			{
				assetDrawer.first->Draw(m_Asset);
			}
		}

	private:
		Ref<Asset> m_Asset;
	};
}
