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

		if (XInputGetState(0, &state) == ERROR_SUCCESS)
		{
			for (BYTE i = 0; i < 255; i++)
			{
				currentState.KeyStates[i] = false;
				if ((int)i == 'K')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
						currentState.KeyStates[i] = true;
					if (state.Gamepad.bLeftTrigger > 230)
						currentState.KeyStates[i] = true;
				}

				if ((int)i == 'S')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
						currentState.KeyStates[i] = true;
					if (state.Gamepad.bRightTrigger > 230)
						currentState.KeyStates[i] = true;
				}

				if ((int)i == 'L')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
						currentState.KeyStates[i] = true;
					if (state.Gamepad.bLeftTrigger > 230)
						currentState.KeyStates[i] = true;
				}

				if ((int)i == 'D')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
						currentState.KeyStates[i] = true;
					if (state.Gamepad.bRightTrigger > 230)
						currentState.KeyStates[i] = true;
				}

				if ((int)i == 'J')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
						currentState.KeyStates[i] = true;
					if (state.Gamepad.bRightTrigger > 230)
						currentState.KeyStates[i] = true;
				}

				if ((int)i == 'A')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
						currentState.KeyStates[i] = true;

					if (state.Gamepad.bLeftTrigger > 230)
						currentState.KeyStates[i] = true;
				}

				if ((int)i == 'I')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
						currentState.KeyStates[i] = true;

					if (state.Gamepad.bLeftTrigger > 230)
						currentState.KeyStates[i] = true;

				}

				if ((int)i == 'W')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
						currentState.KeyStates[i] = true;
					if (state.Gamepad.bRightTrigger > 230)
						currentState.KeyStates[i] = true;

				}

				if ((int)i == 'Q')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
						currentState.KeyStates[i] = true;
				}
				if ((int)i == 'E')
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
						currentState.KeyStates[i] = true;
				}

				if ((int)i == 13)
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
						currentState.KeyStates[i] = true;
				}

				if ((int)i == 0xF0)
				{
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
						currentState.KeyStates[i] = true;
				}
				KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
			}
		}
	}


	bool Xinput::IsDown(BYTE keycode)
	{
		return currentState.IsDown(keycode);
	}

	bool Xinput::IsUp(BYTE keycode)
	{
		return !IsDown(keycode);
	}

	bool Xinput::IsTapped(BYTE keycode)
	{
		return IsDown(keycode) && WasUp(keycode);
	}

	bool Xinput::IsDoubleTapped(BYTE keycode)
	{
		return KeyDoubleTapStates[keycode];
	}

	bool Xinput::IsReleased(BYTE keycode)
	{
		return IsUp(keycode) && WasDown(keycode);
	}

	inline bool Xinput::WasDown(BYTE keycode)
	{
		return lastState.IsDown(keycode);
	}

	inline bool Xinput::WasUp(BYTE keycode)
	{
		return !WasDown(keycode);
	}
}
