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

            const auto prefab = ResourceManager::LoadAsset<PrefabAsset>("PlayerControllerPrefab");
            const auto playerEntity = prefab->Instantiate(scene);
            
            auto& playerTransform = playerEntity.GetComponent<Transform>();
            const auto& startPosition = Vector3(0.0f, 2.0f, 15.0f);
            playerTransform.SetWorldPosition(startPosition);
			
            auto& player = playerEntity.AddComponent<Player>();
            player.Speed = 1.0f;
        }
    };
}
