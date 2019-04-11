#include "KeyboardBinding.h"
#include "../Keyboard/Keyboard.h"

namespace DivaHook::Input
{
	KeyboardBinding::KeyboardBinding(BYTE keycode) : Keycode(keycode)
	{
	}

	KeyboardBinding::~KeyboardBinding()
	{
	}
	
	bool KeyboardBinding::IsDown()
	{
		return Keyboard::GetInstance()->IsDown(Keycode);
	}

	bool KeyboardBinding::IsTapped()
	{
		return Keyboard::GetInstance()->IsTapped(Keycode);
	}

	bool KeyboardBinding::IsReleased()
	{
		return Keyboard::GetInstance()->IsReleased(Keycode);
	}

	bool KeyboardBinding::IsDoubleTapped()
	{
		return Keyboard::GetInstance()->IsDoubleTapped(Keycode);
	}
}
