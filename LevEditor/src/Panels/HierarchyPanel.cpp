#include "pch.h"
#include "HierarchyPanel.h"

#include <imgui.h>

#include "EntitySelection.h"
#include "ModalPopup.h"
#include "Assets/ModelAsset.h"
#include "Assets/PrefabAsset.h"
#include "GUI/EditorGUI.h"
#include "Undo/EntitySnapshot.h"
#include "Undo/UndoCommands.h"
#include "Undo/UndoSystem.h"

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
			if (const Entity entity = entitySelection->Get())
			{
				UndoSystem::Push(CreateRef<EntityDestroyedCommand>(entity,
					Format("Delete {0}", entity.GetName())));

				Scene::DestroyEntity(entity);
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

		if (const void* payload = BeginDragDropTargetWindow(EditorGUI::AssetPayload))
		{
			const Path assetPath = static_cast<const wchar_t*>(payload);

			if (const auto& prefab = AssetDatabase::GetAsset<PrefabAsset>(assetPath))
				RecordEntityCreated(prefab->Instantiate(activeScene), "Instantiate Prefab");

			if (const auto& model = AssetDatabase::GetAsset<ModelAsset>(assetPath))
				RecordEntityCreated(model->InstantiateModel(activeScene), "Instantiate Model");

			ImGui::EndDragDropTarget();
		}

		if (void* payload = BeginDragDropTargetWindow(EditorGUI::EntityPayload))
		{
			if (const auto draggedEntity = *static_cast<Entity*>(payload))
			{
				ScopedEntityEdit edit{ draggedEntity, Format("Move {0}", draggedEntity.GetName()) };
				draggedEntity.GetComponent<Transform>().SetParent(activeScene->GetRootEntity());
			}

			ImGui::EndDragDropTarget();
		}

		const auto rootEntity = activeScene->GetRootEntity();

		//<--- Drawing a node can reparent or delete it, so iterate over a copy ---<<
		const Vector<Entity> rootChildren = rootEntity.GetComponent<Transform>().GetChildren();
		for (const auto child : rootChildren)
		{
			if (!child) continue;

			DrawEntityNode(child);
		}

		if (ImGui::IsMouseReleased(0))
		{
			m_PressedEntity = Entity();
		}

		for (const auto toDelete : m_EntitiesToDelete)
		{
			RecordEntityDestroyed(toDelete, Format("Delete {0}", toDelete.GetName()));

			Scene::DestroyEntity(toDelete);
			Selection::Deselect();
		}
		m_EntitiesToDelete.clear();

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
			Selection::Deselect();

		//Right click on a blank space
		constexpr auto flags = ImGuiPopupFlags_MouseButtonDefault_ | ImGuiPopupFlags_NoOpenOverItems;
		if (ImGui::BeginPopupContextWindow(nullptr, flags))
		{
			if (ImGui::MenuItem("Create New Entity"))
				RecordEntityCreated(activeScene->CreateEntity("New Entity"), "Create Entity");

			ImGui::EndPopup();
		}
	}

	void HierarchyPanel::CreatePrefab(const Entity entity, const Path& path)
	{
		if (const auto& asset = AssetDatabase::CreateNewAsset<PrefabAsset>(path))
		{
			asset->SaveEntity(entity);
			Log::CoreInfo("Prefab '{0}' is created at {1}", entity.GetName(), relative(path, AssetDatabase::GetAssetsPath()).generic_string());
			return;
		}

		Log::CoreWarning("Failed to create prefab '{0}' at {1}", entity.GetName(), relative(path, AssetDatabase::GetAssetsPath()).generic_string());
	}

	void HierarchyPanel::SavePrefab(const Entity entity, const Path& path)
	{
		if (const auto& asset = AssetDatabase::GetAsset<PrefabAsset>(path))
		{
			asset->SaveEntity(entity);
			asset->Deserialize(true);
			Log::CoreInfo("Prefab '{0}' is updated at {1}", entity.GetName(), relative(path, AssetDatabase::GetAssetsPath()).generic_string());
			return;
		}

		Log::CoreWarning("Failed to update prefab '{0}' at {1}", entity.GetName(), relative(path, AssetDatabase::GetAssetsPath()).generic_string());

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
		
		//<--- A double click both selects the entity and brings the scene cameras to it ---<<
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			EntitySelection::SelectEntity(entity);

			if (m_EntityFocused)
				m_EntityFocused(entity);
		}
		else if (ImGui::IsMouseReleased(0))
		{
			if (ImGui::IsItemHovered() && entity == m_PressedEntity)
				EntitySelection::SelectEntity(entity);
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
					ScopedEntityEdit edit{ draggedEntity, Format("Move {0}", draggedEntity.GetName()) };
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
					RecordEntityCreated(child, "Instantiate Prefab");
				}

				if (const auto& prefab = AssetDatabase::GetAsset<ModelAsset>(assetPath))
				{
					const auto child = prefab->InstantiateModel(activeScene);
					child.GetComponent<Transform>().SetParent(entity);
					RecordEntityCreated(child, "Instantiate Model");
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
				RecordEntityCreated(child, "Create Child");
			}

			if (ImGui::MenuItem("Create Parent"))
			{
				//<--- Two changes, one action: undoing puts the entity back where it was and only
				//then takes the parent away, which would otherwise take the entity with it ---<<
				const auto composite = CreateRef<CompositeCommand>(String{ "Create Parent" });

				const auto parent = activeScene->CreateEntity("New Entity");
				composite->Add(CreateRef<EntityCreatedCommand>(parent, String{ "Create Parent" }));

				const String before = EntitySnapshot::Capture(entity);
				entity.GetComponent<Transform>().SetParent(parent);
				composite->Add(CreateRef<EntityStateCommand>(entity.GetUUID(), before,
					EntitySnapshot::Capture(entity), String{ "Create Parent" }));

				UndoSystem::Push(composite);
			}

			if (ImGui::MenuItem("Delete", "delete"))
				m_EntitiesToDelete.emplace(m_EntitiesToDelete.begin(), entity);

			if (ImGui::MenuItem("Duplicate", "ctrl+D"))
				RecordEntityCreated(activeScene->DuplicateEntity(entity), "Duplicate Entity");
			
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

