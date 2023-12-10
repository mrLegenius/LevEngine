

PlayerSpawnSystem = {

	update = function(deltaTime)

		local playerView = Registry.get_entities(Player)

		if playerView:size_hint() > 0 then
			return
		end
		
		local scene = SceneManager:getActiveScene()

		local prefab = Prefab("PlayerPrefab")
		local playerEntity = prefab:instantiate(scene)
		
		local playerTransform = entity:get_component(Transform)
		local startPosition = Vector3(0.0, 1.0, 15.0)
		playerTransform:setWorldPosition(startPosition)
		
		local player = playerEntity:add_component(Player)
		player.speed = 15.0

	end
}