#pragma once
#include <Windows.h>

namespace DivaHook
{
	class MainModule
	{
	public:
		static const wchar_t* DivaWindowName;
		static const wchar_t* GlutDefaultName;

		static HWND DivaWindowHandle;
		static HMODULE Module;
	};
}

