#pragma once
#include <entt/entt.hpp>

#include "System.h"
#include "DataTypes/Delegates.h"
#include "DataTypes/Pointers.h"
#include "DataTypes/Vector.h"
#include "Kernel/UUID.h"

namespace LevEngine
{
	class SceneCamera;
	class Audio;
	class Entity;
	struct Transform;
	
	class Scene
	{
	public:
		Scene();

		void CleanupScene();

		void OnUpdate(float deltaTime);
		void OnPhysics(float deltaTime);
		void OnLateUpdate(float deltaTime);
		void OnRender();
		void OnRender(SceneCamera* mainCamera, const Transform* cameraTransform);

		void OnViewportResized(uint32_t width, uint32_t height);

		void ForEachEntity(const Action<Entity>& callback);

		Entity CreateEntity(const String& name = "Entity");
		Entity CreateEntity(UUID uuid, const String& name);

		void DestroyEntity(entt::entity entity);
		void DestroyEntity(Entity entity);
		static void GetAllChildren(Entity entity, Vector<Entity>& entities);

		Entity DuplicateEntity(Entity entity);
		Entity DuplicateEntity(Entity entity, Entity parent);

		// TODO Not implemented function
		void OnCameraComponentAdded(entt::registry& registry, entt::entity entity);

		template<typename T>
		void RegisterUpdateSystem();

		template<typename T>
		void RegisterLateUpdateSystem();

		template<typename T>
		void RegisterOneFrame();

		template<typename T>
		Entity GetEntityByComponent(T* value);

		Entity GetEntityByUUID(const UUID& uuid);

		entt::registry& GetRegistry();

	private:
		void RequestUpdates(float deltaTime);
		void RequestRenderUpdate();
		void RequestPhysicsUpdates(float deltaTime);
		void RequestRenderUpdate(SceneCamera* mainCamera, const Transform* cameraTransform);
		void RequestLateUpdate(float deltaTime);
		void RequestEventsUpdate(float deltaTime);

		Entity ConvertEntity(entt::entity entity);

	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth;
		uint32_t m_ViewportHeight;

		friend class Entity;

		Vector<Scope<System>> m_UpdateSystems;
		Vector<Scope<System>> m_LateUpdateSystems;
		Vector<Scope<System>> m_EventSystems;

		bool m_IsUpdateDone = true;
		bool m_IsLateUpdateDone = true;
		bool m_IsEventUpdateDone = true;
		bool m_IsPhysicsDone = true;
		bool m_IsRenderDone = true;
		bool m_IsAudioUpdateDone = true;
	};
}

#include "Scene.inl"
