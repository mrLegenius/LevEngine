#include "pch.h"
#include "HierarchyPanel.h"

#include <imgui.h>

#include "EntitySelection.h"
#include "ModalPopup.h"
#include "Assets/PrefabAsset.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
	bool HierarchyPanel::OnKeyPressed(KeyPressedEvent& e)
	{
		LEV_PROFILE_FUNCTION();
		
		const auto& activeScene = SceneManager::GetActiveScene();
		if (!activeScene) return false;
		
		if (e.GetRepeatCount() > 0) return false;

		if (e.GetKeyCode() == KeyCode::Delete)
		{
		if (const auto& entitySelection = Selection::CurrentAs<EntitySelection>())
		{
			if (entitySelection->Get())
			{
				activeScene->DestroyEntity(entitySelection->Get());
				Selection::Deselect();
			}
		}
		}
		return false;
	}

	void HierarchyPanel::DrawContent()
	{
		LEV_PROFILE_FUNCTION();
		
		const auto& activeScene = SceneManager::GetActiveScene();

		if (!activeScene) return;

		if (const ImGuiPayload* payload = BeginDragDropTargetWindow(EditorGUI::AssetPayload))
		{
			const Path assetPath = static_cast<const wchar_t*>(payload->Data);

			if (const auto& prefab = AssetDatabase::GetAsset<PrefabAsset>(assetPath))
				prefab->Instantiate(activeScene);
		}
		
		if (const ImGuiPayload* payload = BeginDragDropTargetWindow(EditorGUI::EntityPayload))
		{
			if (const auto draggedEntity = *static_cast<Entity*>(payload->Data))
				draggedEntity.GetComponent<Transform>().SetParent(Entity{});
		}
		
		activeScene->ForEachEntity(
			[&](const Entity entity)
			{
				if (!entity.GetComponent<Transform>().GetParent())
					DrawEntityNode(entity);
			});

		if (ImGui::IsMouseReleased(0))
		{
			m_PressedEntity = Entity();
		}

		for (const auto toDelete : m_EntitiesToDelete)
		{
			activeScene->DestroyEntity(toDelete);
			Selection::Deselect();
		}
		m_EntitiesToDelete.clear();

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
			Selection::Deselect();

		//Right click on a blank space
		if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
		{
			if (ImGui::MenuItem("Create New Entity"))
				activeScene->CreateEntity("New Entity");

			ImGui::EndPopup();
		}
	}

	void HierarchyPanel::CreatePrefab(const Entity entity, const Path& path)
	{
		if (const auto& asset = AssetDatabase::CreateNewAsset<PrefabAsset>(path))
			asset->SaveEntity(entity);

		Log::CoreInfo("Prefab '{0}' is created at {1}", entity.GetName(), relative(path, AssetDatabase::GetAssetsPath()).generic_string());
	}

	void HierarchyPanel::SavePrefab(const Entity entity, const Path& path)
	{
		if (const auto& asset = AssetDatabase::GetAsset<PrefabAsset>(path))
		{
			asset->SaveEntity(entity);
			asset->Deserialize();
		}

		Log::CoreInfo("Prefab '{0}' is updated at {1}", entity.GetName(), relative(path, AssetDatabase::GetAssetsPath()).generic_string());
	}

	void HierarchyPanel::DrawEntityNode(Entity entity)
	{
		LEV_PROFILE_FUNCTION();

		const auto& activeScene = SceneManager::GetActiveScene();
		
		const auto& tag = entity.GetComponent<TagComponent>().tag;

		const auto entitySelection = Selection::CurrentAs<EntitySelection>();
		
		auto flags =
			(entitySelection && entitySelection->Get() == entity ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_SpanAvailWidth;

		if (entity.GetComponent<Transform>().GetChildrenCount() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		const bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint32_t>(entity)), flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_PressedEntity = entity;
		}
		
		if (ImGui::IsMouseReleased(0))
		{
			if (ImGui::IsItemHovered() && entity == m_PressedEntity)
			{
				if (entitySelection)
					entitySelection->Set(entity);
				else
					Selection::Select(CreateRef<EntitySelection>(entity));
			}
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload(EditorGUI::EntityPayload, &entity, sizeof(Entity), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(EditorGUI::EntityPayload))
			{
				const auto draggedEntity = *static_cast<Entity*>(payload->Data);

				if (entity != draggedEntity)
				{
					draggedEntity.GetComponent<Transform>().SetParent(entity);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(EditorGUI::AssetPayload))
			{
				const Path assetPath = static_cast<const wchar_t*>(payload->Data);

				if (const auto& prefab = AssetDatabase::GetAsset<PrefabAsset>(assetPath))
				{
					const auto child = prefab->Instantiate(activeScene);
					child.GetComponent<Transform>().SetParent(entity);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Child"))
			{
				const auto child = activeScene->CreateEntity("New Entity");
				child.GetComponent<Transform>().SetParent(entity, false);
			}

			if (ImGui::MenuItem("Create Parent"))
			{
				const auto parent = activeScene->CreateEntity("New Entity");
				entity.GetComponent<Transform>().SetParent(parent);
			}
			
			if (ImGui::MenuItem("Delete", "delete"))
				m_EntitiesToDelete.emplace(m_EntitiesToDelete.begin(), entity);
			
			if (ImGui::MenuItem("Duplicate", "ctrl+D"))
				activeScene->DuplicateEntity(entity);
			
			if (ImGui::MenuItem("Save As Prefab", "ctrl+D"))
			{
				Path path = AssetDatabase::GetAssetsPath() / "Prefabs";
				path /= (tag + ".prefab").c_str();

				if (AssetDatabase::AssetExists(path))
				{
					ModalPopup::Show("Prefab already exists", "Do you want to override it?", "Yes", "No",
					[entity, path]
					{
						SavePrefab(entity, path);
					});
				}
				else
				{
					CreatePrefab(entity, path);
				}
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			auto& children = entity.GetComponent<Transform>().GetChildren();

			for (const auto child : children)
			{
				DrawEntityNode(child);
			}
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}
}

