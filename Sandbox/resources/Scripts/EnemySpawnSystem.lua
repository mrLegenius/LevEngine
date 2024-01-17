
local timer = 0
local spawnInterval = 2.0

EnemySpawnSystem = {
	update = function(deltaTime)
		timer = timer + deltaTime;

		if timer >= spawnInterval then
		
			timer = timer - spawnInterval

			local scene = SceneManager:getActiveScene()
			
			local prefab = Prefab("EnemyPrefab")
			local enemyEntity = prefab:instantiate(scene)

			local scriptsContainer = enemyEntity:get_or_add_component(ScriptsContainer)
			scriptsContainer.Enemy = Enemy(5)
			
			local enemyTransform = enemyEntity:get_component(Transform)
			local randomPosition = Random.vector3(-20.0, 20.0)
			randomPosition.y = 1;
			enemyTransform:setWorldPosition(randomPosition)

			Audio.playOneShot("event:/EnemySpawn", enemyEntity)
		end
	end
}