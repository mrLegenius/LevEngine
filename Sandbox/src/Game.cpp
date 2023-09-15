#include "pch.h"
#include "Game.h"

/*
 * There are some problems in this project
 * 1. (Done) We need Project system to have ability change assets of separate project
 * 2. We need Prefabs system to avoid complete object creation from code
 * 3. We need resource loading system (by name)
 * 4. We need our systems and components in editor, to setup entities visually
 * 5. We do not need to control engine pipeline in this project (OnUpdate, OnRender, etc)
 */
namespace Sandbox
{
	int score = 0;
	struct Projectile
	{
		float speed;
		float lifetime;
		float timer;
	};

	struct TargetTag
	{
		float _;
	};
	
	void OnProjectileCollided(Entity entity, Entity other)
	{
		if (!other.HasComponent<TargetTag>()) return;

		SceneManager::GetActiveScene()->DestroyEntity(entity);
		SceneManager::GetActiveScene()->DestroyEntity(other);
		score++;
	}
	
	class FreeCameraSystem : public System
	{
		void Update(const float deltaTime, entt::registry& registry) override
		{
			const auto view = registry.view<Transform, CameraComponent>();

			for (const auto entity : view)
			{
				auto [transform, camera] = view.get<Transform, CameraComponent>(entity);

				if (!camera.isMain) continue;

				if (Input::IsKeyDown(KeyCode::W))
					transform.MoveUp(deltaTime * 10);
				else if (Input::IsKeyDown(KeyCode::S))
					transform.MoveDown(deltaTime * 10);

				if (Input::IsKeyDown(KeyCode::A))
					transform.MoveLeft(deltaTime * 10);
				else if (Input::IsKeyDown(KeyCode::D))
					transform.MoveRight(deltaTime * 10);
			}
		}
	};
	
	class ShootSystem : public System
	{
		void Update(float deltaTime, entt::registry& registry) override
		{
			const auto view = registry.view<Transform, CameraComponent>();

			for (const auto entity : view)
			{
				auto [cameraTransform, camera] = view.get<Transform, CameraComponent>(entity);

				if (!camera.isMain) continue;

				if (Input::IsKeyPressed(KeyCode::Space))
				{
					auto projectile = SceneManager::GetActiveScene()->CreateEntity("Missile");

					auto& transform = projectile.GetComponent<Transform>();

					transform.SetWorldPosition(cameraTransform.GetWorldPosition() + cameraTransform.GetForwardDirection() * 10);
					transform.SetWorldRotation(cameraTransform.GetWorldRotation());

					auto& collider = projectile.AddComponent<SphereCollider>();
					collider.radius = 1;

					auto& events = projectile.AddComponent<CollisionEvents>();
					events.onCollisionBegin.connect<&OnProjectileCollided>();
					auto& rigidbody = projectile.AddComponent<Rigidbody>();
					rigidbody.bodyType = BodyType::Kinematic;
					rigidbody.gravityScale = 0;
					
					auto projectileMeshEntity = SceneManager::GetActiveScene()->CreateEntity("MissileMesh");
					
					auto& meshTransform = projectileMeshEntity.GetComponent<Transform>();
					meshTransform.SetParent(projectile, false);
					meshTransform.SetLocalScale(Vector3::One * 0.05f);
					meshTransform.SetLocalRotation(Random::Rotation());
					
					auto& mesh = projectileMeshEntity.AddComponent<MeshRendererComponent>();
					mesh.mesh = AssetDatabase::GetAsset<MeshAsset>(AssetDatabase::GetAssetsPath() / "Models" / "lava_rock.obj");
					mesh.material = AssetDatabase::GetAsset<MaterialAsset>(AssetDatabase::GetAssetsPath() / "Textures" / "LavaRock" /  "LavaRock.mat");

					auto& projectileComp = projectile.AddComponent<Projectile>();

					projectileComp.speed = 50;
					projectileComp.lifetime = 1;
					projectileComp.timer = 0;
				}
			}
		}
	};

	class ProjectileMovementSystem : public System
	{
	public:
		void Update(float deltaTime, entt::registry& registry) override
		{
			const auto view = registry.view<Transform, Projectile>();

			for (const auto entity : view)
			{
				auto [transform, projectile] = view.get<Transform, Projectile>(entity);

				transform.Move(projectile.speed * deltaTime * transform.GetForwardDirection());
			}
		}
	};

	class ProjectileLifeSystem : public System
	{
	public:
		void Update(const float deltaTime, entt::registry& registry) override
		{
			const auto view = registry.view<Transform, Projectile>();

			for (const auto entity : view)
			{
				auto [transform, projectile] = view.get<Transform, Projectile>(entity);

				projectile.timer += deltaTime;
				
				if (projectile.timer >= projectile.lifetime)
					SceneManager::GetActiveScene()->DestroyEntity(entity);
			}
		}
	};

	class TargetSpawnSystem : public System
	{
	public:
		void Update(const float deltaTime, entt::registry& registry) override
		{
			m_Timer += deltaTime;

			if (m_Timer >= m_SpawnInterval)
			{
				m_Timer -= m_SpawnInterval;

				const auto target = SceneManager::GetActiveScene()->CreateEntity("Target");

				target.AddComponent<TargetTag>();

				auto& rigidbody = target.AddComponent<Rigidbody>();
				rigidbody.bodyType = BodyType::Dynamic;
				rigidbody.gravityScale = 0;
				
				auto& transform = target.GetComponent<Transform>();

				auto randomPosition = Random::Vec3(-5.0f, 5.0f);
				randomPosition.y = 1;
				transform.SetWorldPosition(randomPosition);

				auto& collider = target.AddComponent<BoxCollider>();
				collider.extents = Vector3(0.5, 1, 0.5);
				
				const auto meshEntity = SceneManager::GetActiveScene()->CreateEntity("Mesh");
					
				auto& meshTransform = meshEntity.GetComponent<Transform>();
				meshTransform.SetParent(target, false);
				meshTransform.SetLocalScale(Vector3::One * 0.05f);
					
				auto& mesh = meshEntity.AddComponent<MeshRendererComponent>();
				mesh.mesh = AssetDatabase::GetAsset<MeshAsset>(AssetDatabase::GetAssetsPath() / "FancyTorch" / "FancyTorch.obj");
				mesh.material = AssetDatabase::GetAsset<MaterialAsset>(AssetDatabase::GetAssetsPath() / "FancyTorch" /  "FancyTorch.mat");

			}
		}
	private:
		float m_Timer = 0;
		float m_SpawnInterval = 2.0f;
	};
	
	void Game::OnAttach()
	{
		AssetDatabase::ProcessAllAssets();

		SceneManager::LoadScene(AssetDatabase::GetAssetsPath() / "Scenes" / "TestScene.scene");

		auto& scene = SceneManager::GetActiveScene();

		scene->RegisterUpdateSystem<FreeCameraSystem>();
		scene->RegisterUpdateSystem<ShootSystem>();
		scene->RegisterUpdateSystem<ProjectileMovementSystem>();
		scene->RegisterUpdateSystem<ProjectileLifeSystem>();
		scene->RegisterUpdateSystem<TargetSpawnSystem>();
		scene->RegisterOneFrame<CollisionBeginEvent>();
		scene->RegisterOneFrame<CollisionEndEvent>();
	}

	void Game::OnDetach() { }

	void Game::OnUpdate(const float deltaTime)
	{
		SceneManager::GetActiveScene()->OnUpdate(deltaTime);
		SceneManager::GetActiveScene()->OnPhysics(deltaTime);
		SceneManager::GetActiveScene()->OnLateUpdate(deltaTime);
		SceneManager::GetActiveScene()->OnRender();
	}

	void Game::OnGUIRender()
	{
		const auto time = Time::GetTimeSinceStartup().GetSeconds();
		const String timeStr = String("Time: ") + ToString(time) + String(" sec");
		GUI::DrawString(Vector2(0, 0), timeStr, 16.0f, Color::White);
		
		const String scoreStr = String("Score: ") + ToString(score);
		GUI::DrawString(Vector2(10, 32), scoreStr, 64.0f, Color::White);
		
		GUI::DrawCircle(GUI::GetWindowSize() / 2, 10, Color::White);
	}

	void Game::OnEvent(Event& event) { }
}
