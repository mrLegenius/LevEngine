#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
	class LEV_API FileDialogs
	{
	public:
		//These return empty string if cancelled
		static String OpenFile(const char* filter);
		static String SaveFile(const char* filter, const String& extension);
		static void OpenFileByExtension(const Path& path);
	};
}