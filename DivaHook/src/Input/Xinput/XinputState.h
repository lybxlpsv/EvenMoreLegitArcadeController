#pragma once
#include <windows.h>

namespace DivaHook::Input
{
	struct XinputState
	{
		bool KeyStates[0x3F];
	
		bool IsDown(BYTE keycode);
	};
}

