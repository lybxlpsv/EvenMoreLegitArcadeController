#include "XinputState.h"

namespace DivaHook::Input
{
	bool XinputState::IsDown(BYTE keycode)
	{
		return KeyStates[keycode];
	}
}
