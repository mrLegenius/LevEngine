
rotationSpeed = 45

FPSCameraRotation = {

	update = function(deltaTime)
		local view = Registry.get_entities(Transform, CameraComponent)

		local mouse = Input.getMouseDelta()
		
		delta = mouse * rotationSpeed * deltaTime

		view:for_each(
			function(entity)
				local transform = entity:get_component(Transform)
				local camera = entity:get_component(CameraComponent)

				--print("Found entity", entity:name())

				if not camera.isMain then
					return
				end


				local rotation = transform:getWorldRotation():toEuler() * Math.radToDeg
				rotation.x = rotation.x - delta.y;
				rotation.x = Math.clamp(rotation.x, -89.999, 89.999);

				transform:setWorldRotation(Math.createQuaternionFromYawPitchRoll(rotation * Math.degToRad))
			end
		)
	end
}