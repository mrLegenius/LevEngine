
local rotationSpeed = 45

FPSCameraRotation = {

	update = function(deltaTime)
		local view = Registry.get_entities(Transform, CameraComponent)

		local mouse = Input.getMouseDelta()
		
		local delta = mouse * rotationSpeed * deltaTime

		--print("Kek")

		view:for_each(
			function(entity)
				local transform = entity:get_component(Transform)
				local camera = entity:get_component(CameraComponent)

				if not camera.isMain then
					return
				end

				--print("Found entity", entity:name())

				local rotation = transform:getWorldRotation():toEuler() * Math.radToDeg
				rotation.x = rotation.x - delta.y
				rotation.x = Math.clamp(rotation.x, -89.999, 89.999)
				rotation.y = rotation.y - delta.x;

				transform:setWorldRotation(Math.createQuaternionFromYawPitchRoll(rotation * Math.degToRad))
			end
		)
	end
}