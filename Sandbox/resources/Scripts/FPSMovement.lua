FPSMovement = {
	update = function(deltaTime)
		local playerView = Registry.get_entities(Transform, CharacterController, ScriptsContainer)
        
		local sensitivity = 45.0

		local mouseInput = Input.getMouseDelta()
		
		local mouseDelta = mouseInput * sensitivity * deltaTime

		playerView:for_each(
			function (entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)

				if scriptsContainer.Player == nil then
					return
				end	

				local player = scriptsContainer.Player
				local playerTransform = entity:get_component(Transform)
				local playerController = entity:get_component(CharacterController)
				
				local playerRotation = playerTransform:getWorldRotation():toEuler() * Math.radToDeg
				
				playerRotation.y = playerRotation.y - mouseDelta.x

				playerTransform:setWorldRotation(Math.createQuaternionFromYawPitchRoll(playerRotation * Math.degToRad))
                
				local movementDirection = Vector3.Zero

                if Input.isKeyDown(KeyCode.W) then
                    movementDirection = movementDirection + playerTransform:getForwardDirection()
                elseif Input.isKeyDown(KeyCode.S) then
                    movementDirection = movementDirection - playerTransform:getForwardDirection()
				end		

                if Input.isKeyDown(KeyCode.A) then
                    movementDirection = movementDirection - playerTransform:getRightDirection()
                elseif Input.isKeyDown(KeyCode.D) then
                    movementDirection = movementDirection + playerTransform:getRightDirection()
				end

				movementDirection:normalize()

				local velocity = Vector3.Zero

				if Input.isKeyDown(KeyCode.LeftShift) then
                    velocity = movementDirection * player.sprintSpeed
                else
                    velocity = movementDirection * player.walkSpeed
				end
				
                if Input.isKeyDown(KeyCode.Space) then
                    playerController:jump(player.jumpHeight, deltaTime)
				end

				local displacement = velocity * deltaTime
                
                playerController:move(displacement);		
			end
		)
	end
}