

EnemyMovementSystem = {
	update = function(deltaTime)
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

				local displacement = Vector3.Zero;

				displacement.y = gravity.y * deltaTime;
				
				enemyController:move(displacement, deltaTime);
			end
		)
	end
}