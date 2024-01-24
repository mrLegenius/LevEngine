local patrolTargetSwitchTimer = 0
local wasPatrolTargetSettedOnce = false
local patrolTargetSwitchInterval = 5.0

local perceptUpdateTimer = 0
local perceptUpdateInterval = 0.5

local behaveUpdateTimer = 0
local behaveUpdateInterval = 0.5

AISystem = {
	updatePatrolPosition = function()
		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(agentEntity)
				local agentComponent = agentEntity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end

				local crowdEntity = agentComponent:getCrowd()
				if crowdEntity == nil or not crowdEntity:isValid() then
					return
				end

				if not crowdEntity:has_component(AIAgentCrowdComponent) then
					return
				end

				local crowd = crowdEntity:get_component(AIAgentCrowdComponent)
				if crowd == nil then 
					return
				end

				local randomPosition = crowd:getRandomPointOnNavMesh()
				agentComponent:setFactAsVector3("NextPatrolPosition", randomPosition)
				wasPatrolTargetSettedOnce = true
			end
		)
	end,
	update = function(deltaTime)
		perceptUpdateTimer = perceptUpdateTimer + deltaTime
		behaveUpdateTimer = behaveUpdateTimer + deltaTime
		patrolTargetSwitchTimer = patrolTargetSwitchTimer + deltaTime
		if perceptUpdateTimer >= perceptUpdateInterval then
		
			perceptUpdateTimer = perceptUpdateTimer - perceptUpdateInterval

			AISystem.percept(deltaTime)
		end

		if behaveUpdateTimer >= behaveUpdateInterval then
		
			behaveUpdateTimer = behaveUpdateTimer - behaveUpdateInterval

			AISystem.behave(deltaTime)
		end

	end,
	percept = function()

		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(agentEntity)
				local agentComponent = agentEntity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end
				agentComponent:setFactAsBool("IsPlayerFound", false)
			end
		)
		if not wasPatrolTargetSettedOnce then 
			AISystem.updatePatrolPosition()
		end
		if patrolTargetSwitchTimer >= patrolTargetSwitchInterval then
		
			patrolTargetSwitchTimer = patrolTargetSwitchTimer - patrolTargetSwitchInterval

			AISystem.updatePatrolPosition()
		end
	end,

	behave = function()
		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(entity)
				local agentComponent = entity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end

				if not entity:has_component(ScriptsContainer) then
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

							local conditionName = condition.name
							local conditionValue = condition.value

							local conditionPassed = false

							local conditionType = type(conditionValue)
							
							if conditionType == "boolean" then
								if agentComponent:hasBoolFact(conditionName) then
									if agentComponent:getFactAsBool(conditionName) == conditionValue then
										conditionPassed = true
									end
								end
							end

							if conditionType == "number" then
								if agentComponent:hasNumberFact(conditionName) then
									if agentComponent:getFactAsNumber(conditionName) == conditionValue then
										conditionPassed = true
									end
								end
							end

							if conditionType == "Vector3" then
								if agentComponent:hasVector3Fact(conditionName) then
									if agentComponent:getFactAsVector3(conditionName) == conditionValue then
										conditionPassed = true
									end
								end
							end

							if conditionType == "string" then 
								if agentComponent:hasStringFact(conditionName) then
									if agentComponent:getFactAsString(conditionName) == conditionValue then
										conditionPassed = true
									end
								end
							end

							if not conditionPassed then
								matched = false
								break
							end
						end

						if matched then
							rule.action(agentComponent)
							break
						end
					end
				end
			end
		)
	end
}