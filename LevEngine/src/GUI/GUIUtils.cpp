#include "levpch.h"
#include "GUIUtils.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "TextureLibrary.h"
#include "Assets/AssetDatabase.h"

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

	void GUIUtils::DrawIntControl(const std::string& label,
		const std::function<int()>& getter,
		const std::function<void(int)>& setter,
		const int speed, const int min, const int max)
	{
		auto value = getter();
		if (ImGui::DragInt(label.c_str(), &value, speed, min, max))
			setter(value);
	}

	void GUIUtils::DrawCheckBox(const char* label, const std::function<bool()>& getter,
		const std::function<void(bool)>& setter)
	{
		auto value = getter();
		if (ImGui::Checkbox(label, &value))
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

	void GUIUtils::DrawTextureAsset(const std::string& label, Ref<TextureAsset>* assetPtr)
	{
		DrawAsset(label, assetPtr);
		if (*assetPtr && (*assetPtr)->GetTexture())
		{
			ImGui::SameLine();
			ImGui::Image((*assetPtr)->GetTexture()->GetId(), ImVec2(32, 32), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		}
	}

	void GUIUtils::DrawTexture2D( const std::function<const Ref<Texture>&()>& getter, const std::function<void(Ref<Texture>)>& setter, const Vector2 size)
	{
		const auto texture = getter();
		ImGui::Image(texture ? texture->GetId() : nullptr, ImVec2(size.x, size.y), ImVec2{0, 1}, ImVec2{1, 0});
		if (texture && ImGui::BeginPopupContextItem("Texture2D"))
		{
			if (ImGui::MenuItem("Clear"))
				setter(nullptr);

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetPayload))
			{
				const auto path = static_cast<const wchar_t*>(payload->Data);
				const std::filesystem::path texturePath = AssetDatabase::AssetsRoot / path;
				if (!AssetDatabase::IsAssetTexture(texturePath))
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
}
