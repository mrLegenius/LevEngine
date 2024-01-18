local walkSpeed = 1
local sprintSpeed = 2
local movementMultiplier = 0.25

FPSMovement = {
	update = function(deltaTime)
		local playerView = Registry.get_entities(Transform, CharacterController, ScriptsContainer)
            
		playerView:for_each(
			function (entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)


				if scriptsContainer.Player == nil then
					return
				end	

				local player = scriptsContainer.Player
				local playerTransform = entity:get_component(Transform)
				local playerController = entity:get_component(CharacterController)

				local cameraTransform = playerTransform:getChild(0):get_component(Transform)

				local movement = Vector3.Zero

				

				if Input.isKeyDown(KeyCode.LeftShift) then
                    player.speed = sprintSpeed
                else
                    player.speed = walkSpeed
				end
				player.speed = player.speed * movementMultiplier

				--print(player.speed)
                
                if Input.isKeyDown(KeyCode.W) then
                    movement = movement + Vector3(cameraTransform:getForwardDirection().x, 0.0, cameraTransform:getForwardDirection().z)
					--print("W")
                elseif Input.isKeyDown(KeyCode.S) then
                    movement = movement - Vector3(cameraTransform:getForwardDirection().x, 0.0, cameraTransform:getForwardDirection().z)
					--print("S")
				end

				
                if Input.isKeyDown(KeyCode.A) then
                    movement = movement - Vector3(cameraTransform:getRightDirection().x, 0.0, cameraTransform:getRightDirection().z)
					--print("A")
                elseif Input.isKeyDown(KeyCode.D) then
                    movement = movement + Vector3(cameraTransform:getRightDirection().x, 0.0, cameraTransform:getRightDirection().z)
					--print("D")
				end
                
				
                --[[if Input.isKeyDown(KeyCode.Space) then
                    movement = movement + Vector3.Up
					--print("Space", movement)
				end]]

				movement:normalize()

				--print(movement)

				local displacement = movement * player.speed;
                
                playerController:move(displacement, deltaTime);
			end
		)
	end
}