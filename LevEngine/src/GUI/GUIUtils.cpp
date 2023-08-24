#include "levpch.h"
#include "GUIUtils.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "TextureLibrary.h"

namespace LevEngine
{
	bool GUIUtils::DrawVector3Control(const std::string& label, Vector3& values, const float resetValue, const float columnWidth)
	{
		// -- Init -------------------------------------------------------
		const ImGuiIO& io = ImGui::GetIO();
		const auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		bool changed = false;

		// -- X Component ------------------------------------------------
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushFont(boldFont);

		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			changed = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// -- Y Component ------------------------------------------------
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushFont(boldFont);

		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			changed = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// -- Z Component ------------------------------------------------
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushFont(boldFont);

		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			changed = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		// -- Reset ------------------------------------------------------
		ImGui::PopStyleVar();
		ImGui::Columns();
		ImGui::PopID();

		return changed;
	}

	bool GUIUtils::DrawVector2Control(const std::string& label, Vector2& values, const float resetValue, const float columnWidth)
	{
		// -- Init -------------------------------------------------------
		const ImGuiIO& io = ImGui::GetIO();
		const auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		bool changed = false;

		// -- X Component ------------------------------------------------
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushFont(boldFont);

		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			changed = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// -- Y Component ------------------------------------------------
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushFont(boldFont);

		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			changed = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();

		// -- Reset ------------------------------------------------------
		ImGui::PopStyleVar();
		ImGui::Columns();
		ImGui::PopID();

		return changed;
	}

	void GUIUtils::DrawFloatControl(const std::string& label, float& value, const float speed, const float min, const float max)
	{
		DrawFloatControl(label, [&value] {return value; }, [&value](const float& newValue) {value = newValue; }, speed, min, max);
	}

	void GUIUtils::DrawFloatControl(const std::string& label, const std::function<float()>& getter, const std::function<void(float)>& setter, const float speed, const float min, const float max)
	{
		auto value = getter();
		if (ImGui::DragFloat(label.c_str(), &value, speed, min, max))
			setter(value);
	}

	void GUIUtils::DrawColor3Control(const std::string& label, const std::function<Color()>& getter, const std::function<void(Color)>& setter)
	{
		auto value = getter();
		if (ImGui::ColorEdit3(label.c_str(), value.Raw()))
			setter(value);
	}

	void GUIUtils::DrawTexture2D(Ref<Texture>& texture, const Vector2 size)
	{
		DrawTexture2D([&texture] { return texture; }, [&texture](const Ref<Texture>& newTexture) { texture = newTexture; }, size);
	}

	void GUIUtils::DrawTexture2D( const std::function<const Ref<Texture>&()>& getter, const std::function<void(Ref<Texture>)>& setter, const Vector2 size)
	{
		const auto texture = getter();
		ImGui::Image(texture ? texture->GetId() : nullptr, ImVec2(size.x, size.y), ImVec2{0, 1}, ImVec2{1, 0});
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetPayload))
			{
				const auto path = static_cast<const wchar_t*>(payload->Data);
				const std::filesystem::path texturePath = AssetsPath / path;
				if (!IsAssetTexture(texturePath))
				{
					ImGui::EndDragDropTarget();
					return;
				}

				const Ref<Texture>& newTexture = TextureLibrary::GetTexture(texturePath.string());

				if (newTexture->IsLoaded())
				{
					setter(newTexture);
				}
				else
				{
					Log::CoreWarning("Failed to load texture {0}", texturePath.filename().string());
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	void GUIUtils::DrawAsset(const std::string& label, const std::function<bool(const std::filesystem::path&)>& validation, const std::function<void(const std::filesystem::path&)>& onDrop)
	{
		DrawAsset(label, nullptr, validation, onDrop);
	}

	void GUIUtils::DrawAsset(const std::string& label, const Ref<Asset>& asset, const std::function<bool(const std::filesystem::path&)>& validation, const std::function<void(const std::filesystem::path&)>& onDrop)
	{
		if (!label.empty())
		{
			ImGui::Text(label.c_str());
			ImGui::SameLine();
		}
		
		ImGui::Text(asset ? asset->GetName().c_str() : "None");
		if (!ImGui::BeginDragDropTarget()) return;

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetPayload))
		{
			const auto path = static_cast<const wchar_t*>(payload->Data);
			const std::filesystem::path assetPath = AssetsPath / path;

			if (validation && !validation(path))
			{
				ImGui::EndDragDropTarget();
				return;
			}
			onDrop(assetPath);
		}
		ImGui::EndDragDropTarget();
	}
}
