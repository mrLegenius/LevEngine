

Test = {
	init = function()
	
	end,
	update = function(deltaTime)
		view = Registry.get_entities(Transform)
		--print("Script Update! with deltaTime", deltaTime)
	end,
	lateUpdate = function(deltaTime)
		--print("Script Late Update! with deltaTime", deltaTime)
	end
}