#include "Keyboard.h"
#include <Windows.h>
#include <Xinput.h>

namespace DivaHook::Input
{
	Keyboard* Keyboard::instance;

	Keyboard::Keyboard()
	{
	}

	Keyboard* Keyboard::GetInstance()
	{
		if (instance == nullptr)
			instance = new Keyboard();

		return instance;
	}

	void Keyboard::PollInput()
	{
		lastState = currentState;

		ZeroMemory(&state, sizeof(XINPUT_STATE));
		XInputGetState(0, &state);

		for (BYTE i = 0; i < KEYBOARD_KEYS; i++)
		{
			currentState.KeyStates[i] = GetAsyncKeyState(i) < 0;

			if ((int)i == 'K')
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
					currentState.KeyStates[i] = true;
			}

			if ((int)i == 'S')
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
					currentState.KeyStates[i] = true;
			}

			if ((int)i == 'L')
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
					currentState.KeyStates[i] = true;
			}

			if ((int)i == 'D')
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
					currentState.KeyStates[i] = true;
			}

			if ((int)i == 'A')
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
					currentState.KeyStates[i] = true;
			}

			if ((int)i == 'J')
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
					currentState.KeyStates[i] = true;
			}

			if ((int)i == 'I')
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
					currentState.KeyStates[i] = true;

			}

			if ((int)i == 'W')
			{
				if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
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


			KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
		}
	}

	bool Keyboard::IsDown(BYTE keycode)
	{
		return currentState.IsDown(keycode);
	}

	bool Keyboard::IsUp(BYTE keycode)
	{
		return !IsDown(keycode);
	}

	bool Keyboard::IsTapped(BYTE keycode)
	{
		return IsDown(keycode) && WasUp(keycode);
	}

	bool Keyboard::IsDoubleTapped(BYTE keycode)
	{
		return KeyDoubleTapStates[keycode];
	}

	bool Keyboard::IsReleased(BYTE keycode)
	{
		return IsUp(keycode) && WasDown(keycode);
	}

	inline bool Keyboard::WasDown(BYTE keycode)
	{
		return lastState.IsDown(keycode);
	}

	inline bool Keyboard::WasUp(BYTE keycode)
	{
		return !WasDown(keycode);
	}
}
