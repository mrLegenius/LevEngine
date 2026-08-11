#pragma once
#include "System.h"
#include "DataTypes/Set.h"
#include "Entity.h"

namespace LevEngine
{
	class ScriptAsset;
	class SceneCamera;
	class Audio;
	struct Transform;
	
	class Scene : public eastl::enable_shared_from_this<Scene>
	{
	public:
		Scene();

		void CleanupScene();
		void Initialize();

		void Start();
		void OnUpdate(float deltaTime);
		void OnPhysics(float deltaTime);
		void OnLateUpdate(float deltaTime);
		void OnRender();
		void OnRender(SceneCamera* mainCamera, const Transform* cameraTransform);
		void OnGUIRender();

		void OnViewportResized(uint32_t width, uint32_t height);

		//<--- Parents come before their children, siblings are ordered by child index ---<<
		void ForEachEntity(const Action<Entity>& callback);
		//<--- Cheaper, use it when the order of the entities does not matter ---<<
		void ForEachEntityUnordered(const Action<Entity>& callback);

		Entity CreateEntity(const String& name);
		Entity CreateEntity(const String& name, Entity parent);
		Entity CreateEntity(UUID uuid, const String& name);
		Entity CreateEntity(UUID uuid, const String& name, Entity parent);

		static void DestroyEntity(Entity entity);
		void DestroyAllMarkedEntities();
		void DestroyEntityImmediate(Entity entity);

		Entity DuplicateEntity(Entity entity);
		Entity DuplicateEntity(Entity entity, Entity parent);

		template<typename T>
		void RegisterComponentOnConstruct();

		template<typename T>
		void RegisterComponentOnDestroy();

		template<class T>
		void RegisterInitSystem();
		
		template<typename T>
		void RegisterUpdateSystem();

		template<typename T>
		void RegisterLateUpdateSystem();

		template<typename T>
		void RegisterOneFrame();

		template<typename T>
		void RegisterGUIRenderSystem();

		template<typename T>
		Entity GetEntityByComponent(T* value);

		Entity GetEntityByUUID(const UUID& uuid);

		entt::registry& GetRegistry();

		bool IsScriptSystemActive(const Ref<ScriptAsset>& scriptAsset) const;
		void SetScriptSystemActive(const Ref<ScriptAsset>& scriptAsset, bool isActive);
		Set<Ref<ScriptAsset>> GetActiveScriptSystems() const;
		Entity GetRootEntity() const { return m_RootEntity; }

	private:
		void RequestUpdates(float deltaTime);
		void RequestRenderUpdate();
		void RequestPhysicsUpdates(float deltaTime);
		void RequestRenderUpdate(SceneCamera* mainCamera, const Transform* cameraTransform);
		void RequestLateUpdate(float deltaTime);
		void RequestEventsUpdate(float deltaTime);

		template<typename TComponent>
		void OnComponentConstruct(entt::registry& registry, entt::entity entity);

		template<typename TComponent>
		void OnComponentDestroy(entt::registry& registry, entt::entity entity);

		Entity ConvertEntity(entt::entity entity);

		static void GetAllChildren(Entity entity, Vector<Entity>& entities);
		void GetAllEntities(Vector<Entity>& entities);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth;
		uint32_t m_ViewportHeight;

		Entity m_RootEntity{};

		
		friend class Entity;

		Vector<Scope<System>> m_UpdateSystems;
		Vector<Scope<System>> m_LateUpdateSystems;
		Vector<Scope<System>> m_EventSystems;
		Vector<Scope<System>> m_InitSystems;
		Vector<Scope<System>> m_GUIRenderSystems;

		bool m_IsUpdateDone = true;
		bool m_IsLateUpdateDone = true;
		bool m_IsEventUpdateDone = true;
		bool m_IsPhysicsDone = true;
		bool m_IsRenderDone = true;
		bool m_IsAudioUpdateDone = true;

		Set<Ref<ScriptAsset>> m_ScriptSystems;
	};
}

#include "Scene.inl"
