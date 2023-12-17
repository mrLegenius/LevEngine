Player = {}
Player.__index = Player

setmetatable(Player, {
    __call = function (self, ...)
        return self.new(...)
    end
})

function Player.new(speed)
    local self = setmetatable({}, Player)
    self.speed = speed or 0.0
    return self
end