

ShootSystem = {
	update = function(deltaTime)
		local cameraView = Registry.get_entities(Transform, CameraComponent)

		cameraView:for_each(
			function(entity)
				local camera = entity:get_component(CameraComponent)

				if not camera.isMain then
					return
				end

				local cameraTransform = entity:get_component(Transform)

				if Input.isMouseButtonPressed(MouseButton.Left) then
					local projectilePrefab = Prefab("ProjectilePrefab")
                    local projectile = projectilePrefab:instantiate(SceneManager:getActiveScene())

                    -- TODO: ADD MUZZLE SOCKET
                    local projectileTransform = projectile:get_component(Transform)
                    projectileTransform:setWorldPosition(cameraTransform:getWorldPosition() + cameraTransform:getForwardDirection() * 2)
					
					local scriptsContainer = projectile:get_or_add_component(ScriptsContainer())

					scriptsContainer.Projectile = Projectile()
					scriptsContainer.Projectile.speed = 25
					scriptsContainer.Projectile.lifetime = 2
					scriptsContainer.Projectile.timer = 0

                    -- TODO: NEED TO FIX INITIALIZATION SYSTEM
                    local projectileRigidbody = projectile:get_component(Rigidbody)
                    projectileRigidbody:initialize(projectile)
                    projectileRigidbody:addForce(scriptsContainer.Projectile.speed * cameraTransform:getForwardDirection(), ForceMode.Impulse)
                    
                    Audio.playOneShot("event:/Shot", projectile)
				end
			end
		)
	end
}