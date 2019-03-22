#include "XinputState.h"

namespace DivaHook::Input
{
	bool XinputState::IsDown(int keycode)
	{
		return KeyStates[keycode];
	}
}
