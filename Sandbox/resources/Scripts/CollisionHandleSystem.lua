

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

				local collisions = projectileRigidbody:getCollisionEnterBuffer()

				for k,v in ipairs(collisions) do
				
				    local contactEntity = v.entity;
				
					if contactEntity:has_component(ScriptsContainer) then
						
						scriptsContainer = contactEntity:get_component(ScriptsContainer)
						
						if scriptsContainer.Enemy ~= nil then
						
						    local activeScene = SceneManager.getActiveScene()
							activeScene:destroyEntity(contactEntity)
							activeScene:destroyEntity(entity)
						end
					end
				end

			end
		)
	end
}