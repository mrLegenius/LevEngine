#include "pch.h"
#include "Game.h"

#include "Scene/Systems/Animation/WaypointDisplacementByTimeSystem.h"
#include "Scene/Systems/Animation/WaypointPositionUpdateSystem.h"

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

	struct Player
	{
		float speed;
	};

	struct Enemy
	{
		float speed;
	};
	
	void OnProjectileCollided(Entity entity, Entity other)
	{
		if (!other.HasComponent<Enemy>()) return;

		SceneManager::GetActiveScene()->DestroyEntity(entity);
		SceneManager::GetActiveScene()->DestroyEntity(other);
		score++;
	}
	
	class FPSMovementSystem : public System
	{
		void Update(const float deltaTime, entt::registry& registry) override
		{
			const auto view = registry.view<Transform, Player, Rigidbody>();

			constexpr auto rotationSpeed = 45;
			const Vector2 mouse{ Input::GetMouseDelta().first, Input::GetMouseDelta().second };
			
			for (const auto entity : view)
			{
				auto [transform, player, rigidbody] = view.get<Transform, Player, Rigidbody>(entity);
				
				const auto delta = mouse * rotationSpeed * deltaTime;

				auto rotation = transform.GetWorldRotation().ToEuler() * Math::RadToDeg;
				rotation.y -= delta.x;
				rotation.x -= delta.y;
				transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));
				
				if (Input::IsKeyDown(KeyCode::W))
					transform.MoveForward(deltaTime * 10);
				else if (Input::IsKeyDown(KeyCode::S))
					transform.MoveBackward(deltaTime * 10);

				if (Input::IsKeyDown(KeyCode::A))
					transform.MoveLeft(deltaTime * 10);
				else if (Input::IsKeyDown(KeyCode::D))
					transform.MoveRight(deltaTime * 10);

				if (Input::IsKeyPressed(KeyCode::Space))
					rigidbody.AddImpulse(Vector3::Up * 10);
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

				if (Input::IsMouseButtonPressed(MouseButton::Left))
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

	class EnemySpawnSystem : public System
	{
	public:
		void Update(const float deltaTime, entt::registry& registry) override
		{
			m_Timer += deltaTime;

			if (m_Timer >= m_SpawnInterval)
			{
				m_Timer -= m_SpawnInterval;

				const auto target = SceneManager::GetActiveScene()->CreateEntity("Target");

				auto& enemy = target.AddComponent<Enemy>();
				enemy.speed = 5;

				auto& rigidbody = target.AddComponent<Rigidbody>();
				
				auto& transform = target.GetComponent<Transform>();

				auto randomPosition = Random::Vec3(-25.0f, 25.0f);
				randomPosition.y = 1;
				transform.SetWorldPosition(randomPosition);

				auto& collider = target.AddComponent<BoxCollider>();
				collider.extents = Vector3(0.5, 1.75, 0.5);
				collider.offset = Vector3(0, 1.75, 0);
				
				const auto meshEntity = SceneManager::GetActiveScene()->CreateEntity("Mesh");
					
				auto& meshTransform = meshEntity.GetComponent<Transform>();
				meshTransform.SetParent(target, false);
				meshTransform.SetLocalScale({0.01f, 0.02f, 0.01f});
					
				auto& mesh = meshEntity.AddComponent<MeshRendererComponent>();
				mesh.mesh = AssetDatabase::GetAsset<MeshAsset>(AssetDatabase::GetAssetsPath() / "Enemy" / "enemy_model.fbx");
				mesh.material = AssetDatabase::GetAsset<MaterialAsset>(AssetDatabase::GetAssetsPath() / "Enemy" /  "enemy_material.mat");

			}
		}
	private:
		float m_Timer = 0;
		float m_SpawnInterval = 2.0f;
	};
	
	class PlayerSpawnSystem : public System
	{
	public:
		void Update(float deltaTime, entt::registry& registry) override
		{
			const auto playerView = registry.view<Player>();

			if (!playerView.empty()) return;
			
			auto& scene = SceneManager::GetActiveScene();

			const auto player = scene->CreateEntity("Player");

			auto& playerTransform = player.GetComponent<Transform>();
			playerTransform.SetWorldPosition(Vector3{0, 2, 0});
			
			auto& playerComponent = player.AddComponent<Player>();
			playerComponent.speed = 10;

			auto& rigidbody = player.AddComponent<Rigidbody>();
			auto& collider = player.AddComponent<BoxCollider>();
			collider.extents = Vector3{0.25, 1, 0.25};
			collider.offset = Vector3{0, 1, 0};

			{
				const auto sword = scene->CreateEntity("Sword");

				auto& swordTransform = sword.GetComponent<Transform>();
				swordTransform.SetParent(player);
				swordTransform.SetLocalPosition({0.7f, 2, -4});
				swordTransform.SetLocalScale(Vector3::One * 0.2f);
				const auto eulers = Vector3{10, 100, 0} * Math::DegToRad;
				swordTransform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(eulers));
				
				auto& mesh = sword.AddComponent<MeshRendererComponent>();
				mesh.mesh = AssetDatabase::GetAsset<MeshAsset>(AssetDatabase::GetAssetsPath() / "Sword" / "sword.fbx");
				mesh.material = AssetDatabase::GetAsset<MaterialAsset>(AssetDatabase::GetAssetsPath() / "Sword" / "Sword.mat");
			}

			const auto view = registry.view<Transform, CameraComponent>();
			for (const auto entity : view)
			{
				auto [transform, camera] = view.get<Transform, CameraComponent>(entity);
				
				if (!camera.isMain) continue;

				transform.SetParent(player);
				transform.SetLocalRotation(Quaternion::Identity);
				transform.SetLocalPosition(Vector3{0, 4, 0});
			}
		}
	};

	class EnemyMovementSystem : public System
	{
	public:
		void Update(float deltaTime, entt::registry& registry) override
		{
			const auto enemyView = registry.view<Transform, Rigidbody, Enemy>();

			const auto playerView = registry.view<Transform, Player>();

			Vector3 playerPosition;
			bool foundPlayer = false;
			
			for (const auto player : playerView)
			{
				playerPosition = playerView.get<Transform>(player).GetWorldPosition();
				foundPlayer = true;
			}

			if (!foundPlayer) return;
			
			for (const auto entity : enemyView)
			{
				auto [transform, rigidbody, enemy] = enemyView.get<Transform, Rigidbody, Enemy>(entity);
				
				auto enemyPosition = transform.GetWorldPosition();

				auto dir = playerPosition - enemyPosition;
				dir.y = 0;
				dir.Normalize();

				rigidbody.velocity = dir * enemy.speed;
				
				transform.SetWorldRotation(Quaternion::LookRotation(dir, Vector3::Up));
			}
		}
	};
	
	void Game::OnAttach()
	{
		AssetDatabase::ProcessAllAssets();

		SceneManager::LoadScene(AssetDatabase::GetAssetsPath() / "Scenes" / "TestScene.scene");

		auto& scene = SceneManager::GetActiveScene();

		scene->RegisterUpdateSystem<FPSMovementSystem>();
		scene->RegisterUpdateSystem<ShootSystem>();
		scene->RegisterUpdateSystem<ProjectileMovementSystem>();
		scene->RegisterUpdateSystem<ProjectileLifeSystem>();
		scene->RegisterUpdateSystem<EnemySpawnSystem>();
		scene->RegisterUpdateSystem<PlayerSpawnSystem>();
		scene->RegisterUpdateSystem<EnemyMovementSystem>();
		scene->RegisterUpdateSystem<WaypointDisplacementByTimeSystem>();
		scene->RegisterUpdateSystem<WaypointPositionUpdateSystem>();
		scene->RegisterOneFrame<CollisionBeginEvent>();
		scene->RegisterOneFrame<CollisionEndEvent>();

		Application::Get().GetWindow().DisableCursor();
	}

	void Game::OnDetach() { }

	void Game::OnUpdate(const float deltaTime)
	{
		SceneManager::GetActiveScene()->OnUpdate(deltaTime);
		SceneManager::GetActiveScene()->OnPhysics(deltaTime);
		SceneManager::GetActiveScene()->OnLateUpdate(deltaTime);
		SceneManager::GetActiveScene()->OnRender();

		if (Input::IsKeyPressed(KeyCode::Escape))
			Application::Get().Close();
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
