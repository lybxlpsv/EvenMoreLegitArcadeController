#pragma once
#include "IInputDevice.h"
#include <xinput.h>
#include "../Utilities/Stopwatch.h"

namespace DivaHook::Input
{
	class Xinput : public IInputDevice
	{
		const float DOUBLE_TAP_THRESHOLD = 200.0f;

	public:
		static Xinput* GetInstance();
		
		void PollInput() override; 
		bool IsDown(BYTE keycode);
		bool IsUp(BYTE keycode);
		bool IsTapped(BYTE keycode);
		bool IsReleased(BYTE keycode);

		bool WasDown(BYTE keycode);
		bool WasUp(BYTE keycode);

	private:
		Xinput();
		XINPUT_STATE lastState;
		XINPUT_STATE currentState;

		static Xinput* instance;
	};
}

