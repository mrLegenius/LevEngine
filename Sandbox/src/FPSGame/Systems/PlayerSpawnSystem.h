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
            const auto player = prefab->Instantiate(scene);

            auto& playerTransform = player.GetComponent<Transform>();
            playerTransform.SetWorldPosition(Vector3{0, 1, 15});
			
            auto& playerComponent = player.AddComponent<Player>();
            playerComponent.speed = 10;
        }
    };
}
