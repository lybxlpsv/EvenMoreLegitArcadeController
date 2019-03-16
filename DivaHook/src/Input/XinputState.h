#pragma once
#include <windows.h>

namespace DivaHook::Input
{
	const int KEYBOARD_KEYS = 0xFF;

	struct XinputState
	{
		BYTE KeyStates[KEYBOARD_KEYS];
	
		bool IsDown(BYTE keycode);
	};
}

