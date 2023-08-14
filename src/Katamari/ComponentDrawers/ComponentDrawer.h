#pragma once
#include <imgui.h>
#include <imgui_internal.h>

#include "Kernel/ClassCollection.h"
#include "Scene/Entity.h"

namespace LevEngine
{
	class IComponentDrawer
	{
	public:
		virtual void Draw(Entity entity) = 0;
		virtual void DrawAddComponent(Entity entity) = 0;
	};

	template<typename TComponent, class TDrawer>
	class ComponentDrawer : public IComponentDrawer
	{
	public:
		virtual ~ComponentDrawer() = default;

		void Draw(Entity entity) override
		{
			if (!entity.HasComponent<TComponent>()) return;

			constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_AllowItemOverlap
				| ImGuiTreeNodeFlags_Framed
				| ImGuiTreeNodeFlags_FramePadding;

			auto& component = entity.GetComponent<TComponent>();
			const auto contentRegionAvail = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			const bool opened = ImGui::TreeNodeEx((void*)typeid(TComponent).hash_code(),
				treeNodeFlags, GetLabel().c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);

			if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (IsRemovable() && ImGui::MenuItem("Remove component"))
				{
					removeComponent = true;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			if (opened)
			{
				DrawContent(component);

				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<TComponent>();
		}

		void DrawAddComponent(Entity entity) override
		{
			if (IsRemovable() && !entity.HasComponent<TComponent>())
			{
				if (ImGui::MenuItem(GetLabel().c_str()))
				{
					entity.AddComponent<TComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
		}

	protected:
		[[nodiscard]] virtual std::string GetLabel() const = 0;
		[[nodiscard]] virtual bool IsRemovable() const { return true; }

		virtual void DrawContent(TComponent& component) = 0;

	private:
		static inline ClassRegister<IComponentDrawer, TDrawer> s_ClassRegister;
	};
}
