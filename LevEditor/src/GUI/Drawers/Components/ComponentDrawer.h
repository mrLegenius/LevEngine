#pragma once
#include <imgui.h>
#include <imgui_internal.h>

#include "GUI/EditorGUI.h"
#include "Kernel/ClassCollection.h"
#include "Scene/Entity.h"

namespace LevEngine::Editor
{
	class IComponentDrawer
	{
	public:
		virtual void Draw(Entity entity) = 0;
		virtual void DrawAddComponent(Entity entity) = 0;
	};

	template<typename TComponent, class TDrawer, int Order = 1000>
	class ComponentDrawer : public IComponentDrawer
	{
	public:
		virtual ~ComponentDrawer() = default;

		void Draw(const Entity entity) override
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
			const String label = GetNameFromMenu();
			const bool opened = ImGui::TreeNodeEx((void*)typeid(TComponent).hash_code(),
				treeNodeFlags, label.c_str());

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload(TypeParseTraits<TComponent>::name, &entity, sizeof(Entity), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}
			
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

		void DrawAddComponent(const Entity entity) override
		{
			if (IsRemovable() && !entity.HasComponent<TComponent>())
			{
				DrawMenuItem(GetLabel(), entity);
			}
		}

	protected:
		[[nodiscard]] virtual String GetLabel() const = 0;
		[[nodiscard]] virtual bool IsRemovable() const { return true; }

		virtual void DrawContent(TComponent& component) = 0;

	private:
		[[nodiscard]] String GetNameFromMenu() const
		{
			constexpr char delimiter = '/';
			
			const auto path = GetLabel();
			if (size_t pos; (pos = path.find_last_of(delimiter)) != std::string::npos)
			{
				return path.substr(pos+1, GetLabel().length());
			}

			return path;
		}

		static void DrawMenuItem(String path, const Entity entity)
		{
			constexpr char delimiter = '/';

			if (size_t pos; (pos = path.find(delimiter)) != std::string::npos)
			{
				const String token = path.substr(0, pos);

				if (ImGui::BeginMenu(token.c_str()))
				{
					path.erase(0, pos + 1);
					DrawMenuItem(path, entity);
					ImGui::EndMenu();
				}
				return;
			}
			
			if (ImGui::MenuItem(path.c_str()))
			{
				auto _ = entity.AddComponent<TComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		
		static inline OrderedClassRegister<IComponentDrawer, TDrawer, Order> s_ClassRegister;
	};
}
