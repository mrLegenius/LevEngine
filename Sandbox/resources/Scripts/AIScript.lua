AIScript = {}
AIScript.__index = AIScript

setmetatable(AIScript, {
    __call = function (self, ...)
        return self.new(...)
    end
})

function AIScript.new(variable)
    local self = setmetatable({}, AIScript)
    self.AIScriptVar = variable or 0.0
    return self
end
