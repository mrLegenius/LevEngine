

EnemyMovementSystem = {
	update = function(deltaTime)
		local playerPosition = Vector3()
		local isPlayerFounded = false

		local playerView = Registry.get_entities(Transform, ScriptsContainer)
            
		playerView:for_each(
			function(entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)

				if scriptsContainer.Player ~= nil then
					playerPosition = entity:get_component(Transform):getWorldPosition()
					isPlayerFounded = true
				end
			end
		)
		
		if notisPlayerFounded then
			return
		end

		local enemyView = Registry.get_entities(Transform, CharacterController, ScriptsContainer)
            
		enemyView:for_each(
			function(entity)
				local scriptsContainer = entity:get_component(ScriptsContainer)

				if scriptsContainer.Enemy == nil then
					return
				end

				local enemyTransform = entity:get_component(Transform)
				local enemy = scriptsContainer.Enemy
				local enemyController = entity:get_component(CharacterController)

				local enemyPosition = enemyTransform:getWorldPosition()

				local gravity = Physics.getGravity()

				local direction = playerPosition - enemyPosition
				direction.y = 0.0
				direction:normalize()

				enemyTransform:setWorldRotation(Quaternion.lookRotation(direction, Vector3.Up))

				local displacement = direction * enemy.speed * 0.1;

				--apply gravity
				if displacement.y <= 0.0 then
					displacement.y = gravity.y * deltaTime;
				else
					displacement.y = displacement.y + gravity.y * 10.0 * deltaTime;
				end
				
				enemyController:move(displacement, deltaTime);
			end
		)
	end
}