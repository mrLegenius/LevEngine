

PlayerSpawnSystem = {

	init = function(deltaTime)

		local view = Registry.get_entities(ScriptsContainer)

		-- Check if player already exists

		view:for_each(
			function(entity)
				local scriptsContainer = entity.get_component(ScriptsContainer)

				if scriptsContainer.Player ~= nil then
					return
				end
			end
		)

		-- Spawn Prefab and add Player Component to ScriptsContainer
				
		local scene = SceneManager:getActiveScene()

		local prefab = Prefab("PlayerPrefab")
		local playerEntity = prefab:instantiate(scene)
		
		local playerTransform = playerEntity:get_component(Transform)
		local startPosition = Vector3(0.0, 1.0, 15.0)
		playerTransform:setWorldPosition(startPosition)

		local scriptsContainer = nil
		if not playerEntity:has_component(ScriptsContainer) then
			scriptsContainer = playerEntity:add_component(ScriptsContainer())
		else
			scriptsContainer = playerEntity:get_component(ScriptsContainer)
		end

		scriptsContainer.Player = Player(15.0)

	end
}