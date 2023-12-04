#pragma once

namespace Sandbox
{
    class PlayerSpawnSystem final : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto playerView = registry.view<Player>();
            if (!playerView.empty()) return;
			
            auto& scene = SceneManager::GetActiveScene();

            const auto prefab = ResourceManager::LoadAsset<PrefabAsset>("PlayerPrefab");
            const auto playerEntity = prefab->Instantiate(scene);
            
            auto& transform = playerEntity.GetComponent<Transform>();
            const auto& startPosition = Vector3(0.0f, 1.0f, 15.0f);
            transform.SetWorldPosition(startPosition);
			
            auto& player = playerEntity.AddComponent<Player>();
            player.Speed = 15.0f;

            auto& rigidbody = playerEntity.GetComponent<Rigidbody>();
            rigidbody.SetLinearDamping(1.0f);
            rigidbody.SetMaxLinearVelocity(30.0f);
            rigidbody.LockRotAxisX(true);
            rigidbody.LockRotAxisY(true);
            rigidbody.LockRotAxisZ(true);
            rigidbody.SetStaticFriction(0.5f);
            rigidbody.SetDynamicFriction(0.8f);
            rigidbody.SetRestitution(0.0f);

            // force is used to implement physics-based movement
            auto& force = playerEntity.AddComponent<Force>();
            force.SetForceType(Force::Type::Impulse);
        }
    };
}
