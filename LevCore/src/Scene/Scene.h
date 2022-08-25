#pragma once
#include <entt/entt.hpp>

#include "Renderer/Camera/EditorCamera.h"

namespace LevEngine
{
	class Entity;
	class Scene
	{
	public:
		Scene() = default;
		~Scene();

		void OnUpdateRuntime();
		void OnUpdateEditor(EditorCamera& camera);

		void OnViewportResized(uint32_t width, uint32_t height);
		
		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);
		
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
		Entity GetMainCameraEntity();
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyEditorWindow;
	};
}
