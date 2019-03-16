#include <windows.h>
#include "Xinput.h"

namespace DivaHook::Input
{
	Xinput* Xinput::instance;

	Xinput::Xinput()
	{
		
	}

	Xinput* Xinput::GetInstance()
	{
		if (instance == nullptr)
			instance = new Xinput();

		return instance;
	}

	void Xinput::PollInput()
	{
		lastState = currentState;
		if (XInputGetState(0, &currentState) == ERROR_SUCCESS)
		{
			//TODO: Implement it
		}
		
	}

	bool Xinput::IsDown(BYTE keycode)
	{
		return currentState.Gamepad.wButtons[&keycode] < 0;
	}

	bool Xinput::IsTapped(BYTE keycode)
	{
		return currentState.Gamepad.wButtons[&keycode] < 0;
	}
}
