
local timer = 0
local spawnInterval = 2.0

EnemySpawnSystem = {
	update = function(deltaTime)
		timer = timer + deltaTime;

		if timer >= spawnInterval then
		
			timer = timer - spawnInterval

			local scene = SceneManager:getActiveScene()
			
			local prefab = Prefab("EnemyAgentPrefab")

			local randomPosition = Random.vector3(-20.0, 20.0)
			randomPosition.y = 1;

			local enemyEntity = prefab:instantiate(scene, randomPosition)

			local crowdView = Registry.get_entities(Transform, AIAgentCrowdComponent)
	
			crowdView:for_each(
				function(entity)
					local crowd = entity:get_component(AIAgentCrowdComponent)
					crowd:addAgent(enemyEntity)
				end
			)

			Audio.playOneShot("event:/EnemySpawn", enemyEntity)
		end
	end
}