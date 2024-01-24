AIScript = {}
AIScript.__index = AIScript

setmetatable(AIScript, {
    __call = function (self, ...)
        return self.new(...)
    end
})

function AIScript.new(variable)
    local self = setmetatable({}, AIScript)

    local moveToPlayerPosition = function(agentComponent)
        if agentComponent:hasVector3Fact("PlayerPosiiton") then
            local playerPos = agentComponent:getFactAsVector3("PlayerPosition")
            agentComponent:setMoveTarget(playerPosition)
        end
    end

    local moveToNextPatrolPosition = function(agentComponent)
        if agentComponent:hasVector3Fact("NextPatrolPosition") then
            local nextPatrolPosition = agentComponent:getFactAsVector3("NextPatrolPosition")
            agentComponent:setMoveTarget(nextPatrolPosition)
        end
    end

    self.rules = {}

    self.rules.patrol = 
    {
        conditions = { name = "IsPlayerFound", value = false },
        action = moveToPlayerPosition
    }

    self.rules.moveToPlayer =
    {
        conditions = { name = "IsPlayerFound", value = true },
        action = moveToNextPatrolPosition
    }

    return self
end



