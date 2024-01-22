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
				local playerView = Registry.get_entities(Transform, ScriptsContainer)
				playerView:for_each(
					function(playerEntity)
						local scriptsContainer = playerEntity:get_component(ScriptsContainer)
						if scriptsContainer.Player ~= nil then
							local playerPosition = playerEntity:get_component(Transform):getWorldPosition()
							agentComponent:setFactAsVector3("PlayerPosition", playerPosition)
							agentComponent:setFactAsBool("IsPlayerFound", true)
						end
					end		
				)
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