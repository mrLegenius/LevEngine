Projectile = {}
Projectile.__index = Projectile

setmetatable(Projectile, {
    __call = function (self, ...)
        return self.new(...)
    end
})

function Projectile.new(speed, lifetime, timer)
    local self = setmetatable({}, Projectile)
    self.speed = speed or 0.0
    self.lifetime = lifetime or 0.0
    self.timer = timer or 0.0
    return self
end