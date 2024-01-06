UISystem = {
	onRenderGUI = function(deltaTime)
		local time = Time.getTimeSinceStartup():getSeconds();
		local timeStr = "Time: " .. tostring(time) ..  " sec";
		--GUI.drawString(Vector2(0, 0), timeStr, 16.0, Color.White);
		
		GUI.drawCircle(GUI.getWindowSize() / 2, 10, Color.White);
	end
}