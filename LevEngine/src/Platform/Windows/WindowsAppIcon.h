#pragma once

#include <Windows.h>

namespace LevEngine
{
	namespace WindowsAppIcon
	{
		//The icon is a resource of the executable, not of the engine DLL, so it is
		//always loaded from the process module. Games get their own icon for free by
		//adding an icon resource to their .rc file
		constexpr int k_ResourceId = 1;

		//cx/cy of 0 asks Windows for the size it wants: SM_CXICON for the taskbar and
		//alt-tab, SM_CXSMICON for the title bar, picked from the .ico frames
		//'small' is a macro in the Windows headers, hence the name
		inline HICON Load(const bool smallSize)
		{
			const auto type = smallSize ? SM_CXSMICON : SM_CXICON;
			const int size = GetSystemMetrics(type);

			if (const auto icon = static_cast<HICON>(LoadImage(GetModuleHandle(nullptr),
				MAKEINTRESOURCE(k_ResourceId), IMAGE_ICON, size, size, LR_DEFAULTCOLOR)))
				return icon;

			//No icon resource in the executable
			return LoadIcon(nullptr, IDI_APPLICATION);
		}

		inline HICON LoadBig() { return Load(false); }
		inline HICON LoadSmall() { return Load(true); }
	}
}
