Player = {}
Player.__index = Player

setmetatable(Player, {
    __call = function (self, ...)
        return self.new(...)
    end
})

function Player.new(walkSpeed, sprintSpeed, jumpHeight)
    local self = setmetatable({}, Player)
    self.walkSpeed = walkSpeed or 10.0
    self.sprintSpeed = sprintSpeed or 20.0
    self.jumpHeight = jumpHeight or 3.0
    return self
end