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
            
            auto& playerTransform = playerEntity.GetComponent<Transform>();
            const auto& startPosition = Vector3(0.0f, 1.0f, 15.0f);
            playerTransform.SetWorldPosition(startPosition);
			
            auto& player = playerEntity.AddComponent<Player>();
            player.Speed = 15.0f;

            auto& playerRigidbody = playerEntity.GetComponent<Rigidbody>();
            playerRigidbody.SetLinearDamping(1.0f);
            playerRigidbody.SetMaxLinearVelocity(30.0f);
            playerRigidbody.LockRotAxisX(true);
            playerRigidbody.LockRotAxisY(true);
            playerRigidbody.LockRotAxisZ(true);
            playerRigidbody.SetStaticFriction(0.5f);
            playerRigidbody.SetDynamicFriction(0.8f);
            playerRigidbody.SetRestitution(0.0f);
        }
    };
}
