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
        conditions = { {name = "IsPlayerFound", operation = "==", value = false} },
        actions = {moveToNextPatrolPosition}
    }

    self.rules.moveToPlayer =
    {
        conditions = { {name = "IsPlayerFound", operation = "==", value = true} },
        actions = {moveToPlayerPosition}
    }

    self.rules.moveToAlly =
    {
        conditions = {
             {name = "IsPlayerFound", operation = "~=", value = true},
             {name = "IsBeautifulAllyFounded", operation = "==", value = true},
             {name = "Amorousness", operation = ">", value = 10}
        },
        actions = {moveToPlayerPosition}
    }
    
    return self
end



