FPSCameraRotation = {

	update = function(deltaTime)
		local view = Registry.get_entities(Transform, CameraComponent)

		local mouse = Input.getMouseDelta()
		
		local sensitivity = 45
		
		local delta = mouse * sensitivity * deltaTime

		view:for_each(
			function(entity)
				local camera = entity:get_component(CameraComponent)

				if not camera.isMain then
					return
				end
				
				local transform = entity:get_component(Transform)

				local rotation = transform:getWorldRotation():toEuler() * Math.radToDeg

				rotation.x = rotation.x - delta.y
				rotation.x = Math.clamp(rotation.x, -89.999, 89.999)

				transform:setWorldRotation(Math.createQuaternionFromYawPitchRoll(rotation * Math.degToRad)) 
			end
		)
	end
}