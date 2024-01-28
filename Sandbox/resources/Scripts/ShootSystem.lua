

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
				
                    local startPosition = cameraTransform:getWorldPosition() + cameraTransform:getForwardDirection() * 2;
					local projectilePrefab = Prefab("ProjectilePrefab")
                    local projectile = projectilePrefab:instantiate(SceneManager:getActiveScene(), startPosition)

                    -- TODO: ADD MUZZLE SOCKET
                    local projectileTransform = projectile:get_component(Transform)

					local scriptsContainer = projectile:get_or_add_component(ScriptsContainer)

					scriptsContainer.Projectile = Projectile()
					scriptsContainer.Projectile.speed = 25
					scriptsContainer.Projectile.lifetime = 2
					scriptsContainer.Projectile.timer = 0
					
	            
                    local projectileRigidbody = projectile:get_component(Rigidbody)
                    projectileRigidbody:addForce(scriptsContainer.Projectile.speed * cameraTransform:getForwardDirection(), ForceMode.Impulse)
                    
                    Audio.playOneShot("event:/Shot", projectile)
				end
			end
		)
	end
}