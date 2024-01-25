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
        if agentComponent:hasVector3Fact("PLAYER_*", "PlayerPosition") then
            local playerPos = agentComponent:getFactAsVector3("PLAYER_*","PlayerPosition")
            agentComponent:setMoveTarget(playerPosition)
        end
    end

    local moveToNextPatrolPosition = function(agentComponent)
        if agentComponent:hasVector3Fact("ME", "NextPatrolPosition") then
            local nextPatrolPosition = agentComponent:getFactAsVector3("ME", "NextPatrolPosition")
            agentComponent:setMoveTarget(nextPatrolPosition)
        end
    end

    self.rules = {}

    self.rules.patrol = 
    {
        conditions = { {id = "ME", name = "IsPlayerFound", operation = "==", value = false} },
        actions = {moveToNextPatrolPosition}
    }

    self.rules.moveToPlayer =
    {
        conditions = { {id = "ME", name = "IsPlayerFound", operation = "==", value = true} },
        actions = {moveToPlayerPosition}
    }

    self.rules.moveToAlly =
    {
        conditions = {
             {id = "ME", name = "IsPlayerFound", operation = "~=", value = true},
             {id = "ME", name = "IsBeautifulAllyFounded", operation = "==", value = true},
             {id = "ME", name = "Amorousness", operation = ">", value = 10}
        },
        actions = {moveToPlayerPosition}
    }
    
    return self
end



