#pragma once
#include <windows.h>

namespace DivaHook::Input
{
	struct XinputState
	{
		BYTE KeyStates[255];
	
		bool IsDown(BYTE keycode);
	};
}

