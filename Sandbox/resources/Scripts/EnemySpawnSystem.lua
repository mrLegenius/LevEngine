
local timer = 0
local spawnInterval = 2.0

EnemySpawnSystem = {
	update = function(deltaTime)
		timer = timer + deltaTime;

		if timer >= spawnInterval then
		
			timer = timer - spawnInterval

			local scene = SceneManager:getActiveScene()
			
			local prefab = Prefab("EnemyAgentPrefab")
			local enemyEntity = prefab:instantiate(scene)

			local crowdView = Registry.get_entities(Transform, AIAgentCrowdComponent)

			crowdView:for_each(
				function(entity)
					local crowd = entity:get_component(AIAgentCrowdComponent)
					print("123");
					if crowd == nil then
						print("crowd nil");
					end
					print("234")
					crowd:addAgent(enemyEntity)
					print("456");
				end
			)
	
			local enemyTransform = enemyEntity:get_component(Transform)
			local randomPosition = Random.vector3(-20.0, 20.0)
			randomPosition.y = 1;
			enemyTransform:setWorldPosition(randomPosition)

			Audio.playOneShot("event:/EnemySpawn", enemyEntity)
		end
	end
}