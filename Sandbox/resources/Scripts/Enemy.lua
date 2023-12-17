Enemy = {}
Enemy.__index = Enemy

setmetatable(Enemy, {
    __call = function (self, ...)
        return self.new(...)
    end
})

function Enemy.new(speed)
    local self = setmetatable({}, Enemy)
    self.speed = speed or 0.0
    return self
end