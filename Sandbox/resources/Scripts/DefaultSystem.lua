

Test = {
	init = function()
		print("Script Init")
	end,
	update = function(deltaTime)
		print("Script Update! with deltaTime", deltaTime)
	end,
	lateUpdate = function(deltaTime)
		print("Script Late Update! with deltaTime", deltaTime)
	end
}