#pragma once

namespace Sandbox
{
    class PlayerSpawnSystem final : public System
    {
    public:
        void Update(const float deltaTime, entt::registry& registry) override
        {
            const auto playerView = registry.view<Player>();
            if (!playerView.empty()) return;
			
            auto& scene = SceneManager::GetActiveScene();

            const auto playerPrefab = ResourceManager::LoadAsset<PrefabAsset>("PlayerPrefab");
            const auto playerEntity = playerPrefab->Instantiate(scene);
            
            auto& playerTransform = playerEntity.GetComponent<Transform>();
            const auto& startPosition = Vector3(0.0f, 5.0f, 0.0f);
            playerTransform.SetWorldPosition(startPosition);
			
            auto& player = playerEntity.AddComponent<Player>();
            player.Speed = 1.0f;
        }
    };
}