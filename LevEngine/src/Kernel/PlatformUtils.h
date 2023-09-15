#pragma once

namespace LevEngine
{
	class FileDialogs
	{
	public:
		//These return empty string if cancelled
		static String OpenFile(const char* filter);
		static String SaveFile(const char* filter, const String& extension);
	};
}