#include "Scene.h"

#include "Component.h"
#include "Renderer/Renderer2D.h"
#include "Scene/Entity.h"
#include <glm/gtc/matrix_transform.hpp>

namespace LevEngine
{
	Scene::~Scene()
	{
		m_Registry.clear();
	}

	void Scene::OnUpdateRuntime()
	{		
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& nsc)
			{
				//TODO: Move to Scene::OnSceneStarted
				if (!nsc.instance)
				{
					nsc.instance = nsc.InstantiateScript();
					nsc.instance->m_Entity = Entity{ entity, this };
					nsc.instance->OnCreate();
				}

				nsc.instance->OnUpdate();
			});
		}
		
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		//Render Scene
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.isMain)
				{
					mainCamera = &camera.camera;
					cameraTransform = transform.GetModel();
					break;
				}
			}
		}
		
		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetModel(), sprite, static_cast<int>(entity));
			}


			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		for (auto entity : view)
		{
			auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

			const auto entityID = static_cast<int>(entity);
			Renderer2D::DrawSprite(transform.GetModel(), sprite, entityID);
		}

		Renderer2D::EndScene();
	}


	void Scene::OnViewportResized(const uint32_t width, const uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camera = view.get<CameraComponent>(entity);

			if (camera.fixedAspectRatio) continue;

			camera.camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		auto entity = Entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);

		return entity;
	}

	void Scene::DestroyEntity(const Entity entity)
	{
		m_Registry.destroy(entity);
	}

	Entity Scene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.isMain)
				return Entity(entity, this);
		}
		return { };
	}

	template <typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
			
	}

	template <>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}
	template <>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}
	template <>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template <>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}
	template <>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
}
