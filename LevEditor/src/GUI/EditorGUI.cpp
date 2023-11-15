#include "pch.h"
#include "EditorGUI.h"
#include "ScopedGUIHelpers.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "TextureLibrary.h"
#include "Assets/AssetDatabase.h"

namespace LevEngine::Editor
{
#define IntToFloat(x) ((x) / 255.0f)
	constexpr auto redDark = ImVec4{ IntToFloat(75), IntToFloat(10), IntToFloat(10), 1.0f };
	constexpr auto redRegular = ImVec4{ IntToFloat(140), IntToFloat(20), IntToFloat(20), 1.0f };
	constexpr auto redLight = ImVec4{ IntToFloat(200), IntToFloat(30), IntToFloat(30), 1.0f };

	constexpr auto greenDark = ImVec4{ IntToFloat(10), IntToFloat(75), IntToFloat(10), 1.0f };
	constexpr auto greenRegular = ImVec4{ IntToFloat(20), IntToFloat(140), IntToFloat(20), 1.0f };
	constexpr auto greenLight = ImVec4{ IntToFloat(30), IntToFloat(200), IntToFloat(30), 1.0f };

	constexpr auto blueDark = ImVec4{ IntToFloat(10), IntToFloat(10), IntToFloat(75), 1.0f };
	constexpr auto blueRegular = ImVec4{ IntToFloat(20), IntToFloat(20), IntToFloat(140), 1.0f };
	constexpr auto blueLight = ImVec4{ IntToFloat(30), IntToFloat(30), IntToFloat(200), 1.0f };

	bool EditorGUI::DrawVector3Control(const String& label, Vector3& values, const float resetValue, const float labelWidth)
	{
		// -- Init -------------------------------------------------------
		const ImGuiIO& io = ImGui::GetIO();
		const auto boldFont = io.Fonts->Fonts[0];

		GUI::ScopedID id{label};

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, labelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		GUI::ScopedVariable itemSpacing{ImGuiStyleVar_ItemSpacing, Vector2::Zero};

		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		bool changed = false;

		// -- X Component ------------------------------------------------
		{
			GUI::ScopedColor color1(ImGuiCol_Button, redDark);
			GUI::ScopedColor color2(ImGuiCol_ButtonHovered, redRegular);
			GUI::ScopedColor color3(ImGuiCol_ButtonActive, redLight);
			GUI::ScopedFont font(boldFont);

			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
				changed = true;
			}
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// -- Y Component ------------------------------------------------
		{
			GUI::ScopedColor color1(ImGuiCol_Button, greenDark);
			GUI::ScopedColor color2(ImGuiCol_ButtonHovered, greenRegular);
			GUI::ScopedColor color3(ImGuiCol_ButtonActive, greenLight);
			GUI::ScopedFont font(boldFont);

			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
				changed = true;
			}
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// -- Z Component ------------------------------------------------
		{
			GUI::ScopedColor color1(ImGuiCol_Button, blueDark);
			GUI::ScopedColor color2(ImGuiCol_ButtonHovered, blueRegular);
			GUI::ScopedColor color3(ImGuiCol_ButtonActive, blueLight);
			GUI::ScopedFont font(boldFont);

			if (ImGui::Button("Z", buttonSize))
			{
				values.z = resetValue;
				changed = true;
			}
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		// -- Reset ------------------------------------------------------
		ImGui::Columns();

		return changed;
	}

	void EditorGUI::DrawVector3Control(const String& label, const Func<Vector3>& getter, const Action<Vector3>& setter, const float speed, const float min, const float max)
	{
		auto value = getter();
		float* v = &(value.x);
		if (ImGui::DragFloat3(label.c_str(), v, speed, min, max))
			setter(value);
	}
	
	bool EditorGUI::DrawVector2Control(const String& label, Vector2& values, const float resetValue, const float labelWidth)
	{
		// -- Init -------------------------------------------------------
		const ImGuiIO& io = ImGui::GetIO();
		const auto boldFont = io.Fonts->Fonts[0];

		GUI::ScopedID id{label};

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, labelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		GUI::ScopedVariable itemSpacing{ImGuiStyleVar_ItemSpacing, Vector2::Zero};

		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		bool changed = false;

		// -- X Component ------------------------------------------------
		{
			GUI::ScopedColor color1(ImGuiCol_Button, redDark);
			GUI::ScopedColor color2(ImGuiCol_ButtonHovered, redRegular);
			GUI::ScopedColor color3(ImGuiCol_ButtonActive, redLight);
			GUI::ScopedFont font(boldFont);

			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
				changed = true;
			}
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// -- Y Component ------------------------------------------------
		{
			GUI::ScopedColor color1(ImGuiCol_Button, greenDark);
			GUI::ScopedColor color2(ImGuiCol_ButtonHovered, greenRegular);
			GUI::ScopedColor color3(ImGuiCol_ButtonActive, greenLight);
			GUI::ScopedFont font(boldFont);

			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
				changed = true;
			}
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		
		// -- Reset ------------------------------------------------------
		ImGui::Columns();

		return changed;
	}

	void EditorGUI::DrawVector2Control(const String& label, const Func<Vector2>& getter, const Action<Vector2>& setter,float speed, float min, float max)
	{
		auto value = getter();
		float* v = &(value.x);
		if (ImGui::DragFloat2(label.c_str(), v, speed, min, max))
			setter(value);
	}

	void EditorGUI::DrawFloatControl(const String& label, float& value, const float speed, const float min, const float max)
	{
		DrawFloatControl(label, [&value] {return value; }, [&value](const float& newValue) {value = newValue; }, speed, min, max);
	}

	void EditorGUI::DrawFloatControl(const String& label, const Func<float>& getter, const Action<float>& setter, const float speed, const float min, const float max)
	{
		auto value = getter();
		if (ImGui::DragFloat(label.c_str(), &value, speed, min, max))
			setter(value);
	}

	void EditorGUI::DrawDoubleControl(const String& label, double& value, double speed, double min, double max)
	{
		DrawDoubleControl(label, [&value] {return value; },
			[&value](const float& newValue) {value = newValue; }, speed, min, max);
	}

	void EditorGUI::DrawDoubleControl(const String& label, const Func<double>& getter,
		const Action<double>& setter, double speed, double min, double max)
	{
		auto value = getter();
		if (ImGui::DragScalar(label.c_str(), ImGuiDataType_Double, &value, speed, &min, &max))
			setter(value);
	}

	void EditorGUI::DrawIntControl(const String& label,
		const Func<int>& getter,
		const Action<int>& setter,
		const int speed, const int min, const int max)
	{
		auto value = getter();
		if (ImGui::DragInt(label.c_str(), &value, static_cast<float>(speed), min, max))
			setter(value);
	}

	void EditorGUI::DrawColor3Control(const String& label, Color& value)
	{
		ImGui::ColorEdit3(label.c_str(), value.Raw());
	}

	bool EditorGUI::DrawCheckBox(const char* label, const Func<bool>& getter,
	                             const Action<bool>& setter)
	{
		auto value = getter();
		if (DrawCheckBox(label, value))
		{
			setter(value);
			return true;
		}
		
		return false;
	}

	bool EditorGUI::DrawCheckBox(const char* label, bool& value)
	{
		if (ImGui::Checkbox(label, &value))
			return true;

		return false;
	}

	void EditorGUI::DrawColor3Control(const String& label, const Func<Color>& getter, const Action<Color>& setter)
	{
		auto value = getter();
		if (ImGui::ColorEdit3(label.c_str(), value.Raw()))
			setter(value);
	}

	void EditorGUI::DrawTexture2D(Ref<Texture>& texture, const Vector2 size)
	{
		DrawTexture2D([&texture] { return texture; }, [&texture](const Ref<Texture>& newTexture) { texture = newTexture; }, size);
	}

	bool EditorGUI::DrawTextureAsset(const String& label, Ref<TextureAsset>& assetPtr)
	{
		const bool changed = DrawAsset(label, assetPtr);
		if (assetPtr && assetPtr->GetTexture())
		{
			ImGui::SameLine();
			ImGui::Image(assetPtr->GetTexture()->GetId(), ImVec2(32, 32), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		}

		return changed;
	}

	bool EditorGUI::DrawTextureAsset(Ref<TextureAsset>& assetPtr, const Vector2 size)
	{
		auto changed = false;
		const auto& asset = assetPtr;

		const auto& texture = asset ? asset->GetTexture() : nullptr;
		ImGui::Image(texture ? texture->GetId() : nullptr, ImVec2(size.x, size.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (asset && ImGui::BeginPopupContextItem("Asset"))
		{
			if (ImGui::MenuItem("Clear"))
			{
				assetPtr = nullptr;
				changed = true;
			}
			ImGui::EndPopup();
		}

		if (!ImGui::BeginDragDropTarget()) return changed;

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetPayload))
		{
			const Path assetPath = static_cast<const wchar_t*>(payload->Data);

			if (const auto newAsset = AssetDatabase::GetAsset<TextureAsset>(assetPath))
			{
				assetPtr = newAsset;
				changed = true;
			}
		}
		ImGui::EndDragDropTarget();

		return changed;
	}

	void EditorGUI::DrawTexture2D(const Func<Ref<Texture>>& getter, const Action<Ref<Texture>>& setter, const Vector2 size)
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
				const Path texturePath = static_cast<const wchar_t*>(payload->Data);
				if (!AssetDatabase::IsAssetTexture(texturePath))
				{
					ImGui::EndDragDropTarget();
					return;
				}

				const Ref<Texture> newTexture = TextureLibrary::GetTexture(texturePath.string().c_str());

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
