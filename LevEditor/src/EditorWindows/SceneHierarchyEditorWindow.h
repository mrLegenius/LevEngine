#pragma once
#include "LevEngine.h"

namespace LevEngine
{
	class SceneHierarchyEditorWindow
	{
	public:
		SceneHierarchyEditorWindow() = default;
		SceneHierarchyEditorWindow(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		void OnImGuiRender();

		
		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(const Entity entity) { m_SelectionContext = entity; }
	private:
		Ref<Scene> m_Context = nullptr;
		Entity m_SelectionContext;

		friend class Scene;
	};
}
