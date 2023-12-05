#pragma once

namespace Sandbox
{
    class EnemySpawnSystem final : public System
    {
    public:
        void Update(const float deltaTime, entt::registry& registry) override
        {
            m_Timer += deltaTime;

            if (m_Timer >= m_SpawnInterval)
            {
                m_Timer -= m_SpawnInterval;

                const auto scene = SceneManager::GetActiveScene();
                
                const auto prefab = ResourceManager::LoadAsset<PrefabAsset>("EnemyPrefab");
                const auto enemyEntity = prefab->Instantiate(scene);

                auto& enemy = enemyEntity.AddComponent<Enemy>();
                enemy.Speed = 5;
				
                auto& enemyTransform = enemyEntity.GetComponent<Transform>();
                auto randomPosition = Random::Vec3(-20.0f, 20.0f);
                randomPosition.y = 1;
                enemyTransform.SetWorldPosition(randomPosition);

                Audio::PlayOneShot("event:/EnemySpawn", enemyEntity);
            }
        }
    private:
        float m_Timer = 0;
        float m_SpawnInterval = 2.0f;
    };
}
