AnimationSwitcherSystem = {
	update = function(deltaTime)
		local animatorView = Registry.get_entities(Transform, AnimatorComponent)

		animatorView:for_each(
			function(entity)
				local animator = entity:get_component(AnimatorComponent)
				
				if Input.isMouseButtonPressed(MouseButton.Left) then
					animator:setAnimationClip("Dance1")
					animator:playAnimation()
				end

				if Input.isMouseButtonPressed(MouseButton.Right) then
					animator:setAnimationClip("Dance2")
					animator:playAnimation()
				end
			end
		)
		
	end,
	
}