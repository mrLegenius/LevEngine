

PlayerSpawnSystem = {

	init = function(deltaTime)

		local view = Registry.get_entities(ScriptsContainer)

		-- Check if player already exists

		print("Really long message, which should be wraped as it gets longer and longer and longer and longer and longer and longer and longer and longer and longer and longer and longer")

		local alreadyExists = false

		view:for_each(
			function(entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)

				if scriptsContainer.Player ~= nil then
					alreadyExists = true
				end
			end
		)

		if alreadyExists then
			return
		end

		-- Spawn Prefab and add Player Component to ScriptsContainer
				
		local scene = SceneManager:getActiveScene()

		local prefab = Prefab("PlayerPrefab")
		local playerEntity = prefab:instantiate(scene)
		
		local playerTransform = playerEntity:get_component(Transform)
		local startPosition = Vector3(0.0, 1.0, 15.0)
		playerTransform:setWorldPosition(startPosition)

		local scriptsContainer = playerEntity:get_or_add_component(ScriptsContainer())
		
		scriptsContainer.Player = Player(15.0)
	end
}