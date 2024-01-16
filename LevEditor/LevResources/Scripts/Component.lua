Component = {}
Component.__index = Component

setmetatable(Component, {
    __call = function (self, ...)
        return self.new(...)
    end
})

function Component.new(variable)
    local self = setmetatable({}, variable)
    self.componentVar = variable or 0.0
    return self
end
