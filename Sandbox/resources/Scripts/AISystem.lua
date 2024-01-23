AISystem = {
	update = function(deltaTime)
		AISystem.percept(deltaTime)
		AISystem.behave(deltaTime)
	end,
	percept = function(deltaTime)
		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(agentEntity)
				local agentComponent = agentEntity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end
				local agentCharacterController = agentEntity:get_component(CharacterController)
				if agentCharacterController == nil then
					return
				end
				local agentTransform = agentEntity:get_component(Transform)
				local playerEntity = agentComponent:findEntityInVisibleScope(FilterLayer.Layer2)
				local isValid = playerEntity:isValid()
				if isValid then
					agentComponent:setFactAsBool("IsPlayerFound", true)
					local playerTransform = playerEntity:get_component(Transform)
					local playerPosition = playerTransform:getWorldPosition()
					agentComponent:setFactAsVector3("PlayerPosition", playerPosition)
				end
			end
		)
	end,
	behave = function(deltaTime)
		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(entity)
				local agentComponent = entity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end
				if agentComponent:hasBoolFact("IsPlayerFound") and agentComponent:getFactAsBool("IsPlayerFound") then
					if(agentComponent:hasVector3Fact("PlayerPosition")) then
						local playerPosition = agentComponent:getFactAsVector3("PlayerPosition")
						agentComponent:setMoveTarget(playerPosition)
					end
				end
			end
		)
	end
}