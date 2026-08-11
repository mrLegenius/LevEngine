#pragma once

// LevEngine is built as a shared library (LevEngine.dll) so that LevEditor and game
// projects link against one shared copy of the engine.
//
// LEV_BUILD_DLL is defined by LevEngine.vcxproj only, so the engine exports its public
// API while every client (LevEditor, Sandbox, ...) imports it.
//
// Usage:
//     class LEV_API Foo { ... };          // whole class: members, statics and vtable
//     LEV_API void Bar();                 // free function
//     extern LEV_API int g_Baz;           // global variable
//
// Class templates must NOT be marked: they are instantiated on the client side and have
// no symbol to import. The same goes for anything defined entirely inline in a header
// that no client needs to link against.

#ifndef LEV_API
#	if defined(_MSC_VER)
#		if defined(LEV_BUILD_DLL)
#			define LEV_API __declspec(dllexport)
#		else
#			define LEV_API __declspec(dllimport)
#		endif
#	elif defined(__GNUC__)
#		define LEV_API __attribute__((visibility("default")))
#	else
#		define LEV_API
#	endif
#endif
