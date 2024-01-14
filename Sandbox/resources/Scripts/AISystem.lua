--[[FACTS]]
local playerPosition = Vector3()
local isPlayerFounded = false

AISystem = {
	update = function(deltaTime)
		AISystem.percept(deltaTime)
		AISystem.behave(deltaTime)
	end,
	percept = function(deltaTime)
		local playerView = Registry.get_entities(Transform, ScriptsContainer)
		playerView:for_each(
			function(entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)

				if scriptsContainer.Player ~= nil then
					playerPosition = entity:get_component(Transform):getWorldPosition()
					isPlayerFounded = true
				end
			end
		)
	end,
	behave = function(deltaTime)
		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(entity)
				local characterController = entity:get_component(CharacterController)
				if characterController == nil then
					return
				end
				local agentComponent = entity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end
				local characterController
				if isPlayerFounded then
					playerPosition
					agentComponent:setMoveTarget(playerPosition)
				end
			end
		)
	end
}