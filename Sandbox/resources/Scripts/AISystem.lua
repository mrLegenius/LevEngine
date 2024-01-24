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

				local scriptsContainer = entity:get_component(ScriptsContainer)

				local aiScript = scriptsContainer.AIScript
			
				if aiScript ~= nil then
					local rules = aiScript.rules
					for ruleKey, rule in pairs(rules) do
						--итерация по правилам
						local matched = true
						for conditionKey, condition in pairs(rule.conditions) do
							--итерация по условиям правила
							local conditionName = condition[0]
							local conditionValue = condition[1]

							local conditionPassed = false

							if type(conditionValue) == "boolean" then
								if agentComponent:hasBoolFact(conditionName) then
									if agentComponent:getFactAsBool(conditionName) == conditionValue then
										conditionPassed = true
									end
								end
							end

							if type(conditionValue) == "number" then
								if agentComponent:hasNumberFact(conditionName) then
									if agentComponent:getFactAsNumber(conditionName) == conditionValue then
										conditionPassed = true
									end
								end
							end

							if type(conditionValue) == "Vector3" then
								if agentComponent:hasVector3Fact(conditionName) then
									if agentComponent:getFactAsVector3(conditionName) == conditionValue then
										conditionPassed = true
									end
								end
							end

							if type(conditionValue) == "string" then 
								if agentComponent:hasStringFact(conditionName) then
									if agentComponent:getFactAsString(conditionName) == conditionValue then
										conditionPassed = true
									end
								end
							end

							if notconditionPassed then
								matched = false
								break
							end
						end
						if matched then
							rulesv.action()
						end
					end
				end
			end
		)
	end
}