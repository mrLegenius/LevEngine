

CollisionHandleSystem = {
	update = function(deltaTime)
		local projectileView = Registry.get_entities(Transform, Rigidbody, ScriptsContainer)

		projectileView:for_each(
			function(entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)

				if scriptsContainer.Projectile == nil then
					return
				end

				

				local projectileTransform = entity:get_component(Transform)
				local projectileRigidbody = entity:get_component(Rigidbody)
				local projectile = scriptsContainer.Projectile

				local scene = SceneManager:getActiveScene()

				
				local collisions = projectileRigidbody:getCollisionEnterBuffer()

				for k,v in ipairs(collisions) do
					if v.contactEntity:has_component(ScriptsContainer) then
						
						--print(v.contactEntity:name())
						scriptsContainer = v.contactEntity:get_component(ScriptsContainer)
						
						if scriptsContainer.Enemy ~= nill then
							scene:destroyEntity(v.contactEntity)
							scene:destroyEntity(entity)
						end
					end
				end

			end
		)
	end
}