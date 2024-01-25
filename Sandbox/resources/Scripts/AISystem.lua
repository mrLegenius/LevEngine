local perceptUpdateTimer = 0
local perceptUpdateInterval = 0.5

local behaveUpdateTimer = 0
local behaveUpdateInterval = 0.5

local patrolTargetSwitchInterval = 4

AISystem = {
	updatePatrol = function(deltaTime)
		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(agentEntity)
				local agentComponent = agentEntity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end

				if agentComponent:hasNumberFact("ME", "TimeFromLastPatrolTargetSwitch") then
					local timeFromLastPatrolTargetSwitch = agentComponent:getFactAsNumber("ME", "TimeFromLastPatrolTargetSwitch")
					if timeFromLastPatrolTargetSwitch >= patrolTargetSwitchInterval then
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
						agentComponent:setFactAsVector3("ME", "NextPatrolPosition", randomPosition)
						agentComponent:setFactAsNumber("ME", "TimeFromLastPatrolTargetSwitch", 0)
						local abi = agentComponent:getFactAsNumber("ME", "TimeFromLastPatrolTargetSwitch")
					else
						local incrementedTime = timeFromLastPatrolTargetSwitch + deltaTime
						agentComponent:setFactAsNumber("ME", "TimeFromLastPatrolTargetSwitch", incrementedTime)
					end
				else
					agentComponent:setFactAsNumber("ME", "TimeFromLastPatrolTargetSwitch", patrolTargetSwitchInterval)
				end
			end
		)
	end,
	update = function(deltaTime)
		perceptUpdateTimer = perceptUpdateTimer + deltaTime
		behaveUpdateTimer = behaveUpdateTimer + deltaTime

		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(agentEntity)
				local agentComponent = agentEntity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end

				if not agentEntity:has_component(ScriptsContainer) then
					return
				end

				local scriptsContainer = agentEntity:get_component(ScriptsContainer)

				local aiScript = scriptsContainer.AIScript
				
				if aiScript ~= nil then
					if not agentComponent:isRBSInited() then 
						local rules = aiScript.rules
						for ruleKey, rule in pairs(rules) do
							local ruleObject = Rule(ruleKey)
							for conditionKey, condition in pairs(rule.conditions) do
								ruleObject:addCondition(condition.id, condition.name, condition.operation, condition.value)
							end
							agentComponent:addRule(ruleObject)
						end
						agentComponent:initRBS()
					end
				end
			end
		)

		if perceptUpdateTimer >= perceptUpdateInterval then
		
			AISystem.percept(perceptUpdateTimer)
			perceptUpdateTimer = perceptUpdateTimer - perceptUpdateInterval
		end

		if behaveUpdateTimer >= behaveUpdateInterval then
		
			AISystem.behave(behaveUpdateTimer)
			behaveUpdateTimer = behaveUpdateTimer - behaveUpdateInterval
		end
	end,
	percept = function(deltaTime)

		local aiAgentsView = Registry.get_entities(Transform, CharacterController, AIAgentComponent)
		aiAgentsView:for_each(
			function(agentEntity)
				local agentComponent = agentEntity:get_component(AIAgentComponent)
				if agentComponent == nil then
					return
				end
				agentComponent:setFactAsBool("ME", "IsPlayerFound", false)
			end
		)
		AISystem.updatePatrol(deltaTime)
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
					local matchedRule = agentComponent:match()
					local actions = aiScript.rules[matchedRule].actions
					for actionKey, action in pairs(actions) do
						action(agentComponent)
					end
				end
			end
		)
	end
}