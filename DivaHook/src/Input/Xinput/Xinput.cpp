#include <windows.h>
#include "Xinput.h"
#include "../../Constants.h"

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

	bool Xinput::PollInput()
	{
		lastState = currentState;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(0, &state) == ERROR_SUCCESS)
		{
				BYTE i = XINPUT_A;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_DOWN;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_B;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_RIGHT;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_X;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_LEFT;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_Y;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_UP;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_LS;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_RS;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_START;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_BACK;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_LT;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.bLeftTrigger > 230)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}

				i = XINPUT_RT;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.bRightTrigger > 230)
						currentState.KeyStates[i] = true;
					KeyDoubleTapStates[i] = IsTapped(i) ? KeyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;
				}
		}
		else {
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		ZeroMemory(&currentState, sizeof(currentState));
		}
			return TRUE;
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
