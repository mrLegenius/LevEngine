#include "levpch.h"
#include "Kernel/PlatformUtils.h"

#include "DataTypes/Path.h"
#include "Kernel/Application.h"

namespace LevEngine
{
	String FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)Application::Get().GetWindow().GetNativeWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		
		return GetOpenFileNameA(&ofn) == TRUE ? ofn.lpstrFile : String();
	}

	String FileDialogs::SaveFile(const char* filter, const String& extension)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)Application::Get().GetWindow().GetNativeWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn) == FALSE) return {};
		
		Path result = ofn.lpstrFile;
		if (!extension.empty() && !result.has_extension() || result.extension().string() != std::string(extension.c_str()))
		{
			result = result.string().append(".").append(extension.c_str());
		}
		return result.string().c_str();
	}
}