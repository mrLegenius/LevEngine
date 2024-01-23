Enemy = {}
Enemy.__index = Enemy

setmetatable(Enemy, {
    __call = function (self, ...)
        return self.new(...)
    end
})

function Enemy.new(variable)
    local self = setmetatable({}, Enemy)
    return self
end
