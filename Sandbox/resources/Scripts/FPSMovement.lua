local walkSpeed = 15
local sprintSpeed = 30
local jumpForce = 50

FPSMovement = {
	update = function(deltaTime)
		local playerView = Registry.get_entities(Transform, Rigidbody, ScriptsContainer)
            
		playerView:for_each(
			function (entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)

				if scriptsContainer.Player == nil then
					return
				end	

				local player = scriptsContainer.Player
				local playerTransform = entity:get_component(Transform)
				local playerRigidbody = entity:get_component(Rigidbody)

				local cameraTransform = playerTransform:getChild(0):get_component(Transform)

				local movement = Vector3.Zero

				if Input.isKeyDown(KeyCode.LeftShift) then
                    player.speed = sprintSpeed
                else
                    player.speed = walkSpeed
				end
                
                if Input.isKeyDown(KeyCode.W) then
                    movement = movement + Vector3(cameraTransform.getForwardDirection().x, 0.0, cameraTransform.getForwardDirection().z)
                elseif Input.isKeyDown(KeyCode.S) then
                    movement = movement - Vector3(cameraTransform.getForwardDirection().x, 0.0, cameraTransform.getForwardDirection().z)
				end

                if Input.IsKeyDown(KeyCode.A) then
                    movement = movement - Vector3(cameraTransform.getRightDirection().x, 0.0, cameraTransform.getRightDirection().z)
                elseif Input.isKeyDown(KeyCode.D) then
                    movement = movement + Vector3(cameraTransform.getRightDirection().x, 0.0, cameraTransform.getRightDirection().z)
				end
                
                if Input.isKeyDown(KeyCode.Space) then
                    movement = movement + Vector3.Up
				end

				movement:normalize()

				print(movement.x, movement.y, movement.z)

				playerRigidbody:addForce(
                    Vector3(
                        deltaTime * player.speed * movement.x,
                        deltaTime * jumpForce * movement.y,
                        deltaTime * player.speed * movement.z
                    ),
                    ForceMode.Impulse
				)
			end
		)
	end
}