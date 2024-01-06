UISystem = {
	GUIRender = function()
		local time = Time.getTimeSinceStartup():getSeconds();
		local timeStr = "Time: " .. tostring(time) ..  " sec";
		GUI.drawString(Vector2(0, 0), timeStr, 16.0, Color.White);
		
		GUI.drawCircle(Vector2(0.5, 0.5), 10, Color.White);
	end
}