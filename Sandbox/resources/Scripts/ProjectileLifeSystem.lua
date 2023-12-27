

ProjectileLifeSystem = {
	update = function(deltaTime)
		local projectileView = Registry.get_entities(Transform, ScriptsContainer)

		projectileView:for_each(
			function(entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)

				if scriptsContainer.Projectile == nil then
					return
				end

				local projectile = scriptsContainer.Projectile

				projectile.timer = projectile.timer + deltaTime
				--print(projectile.timer, projectile.lifetime)
                if projectile.timer >= projectile.lifetime then
					--print("Kill", entity:name())
                    SceneManager:getActiveScene():destroyEntity(entity)
				end

			end
		)
	end
}