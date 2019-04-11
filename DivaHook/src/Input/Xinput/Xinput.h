#pragma once
#include "../IInputDevice.h"
#include <xinput.h>
#include "../../Utilities/Stopwatch.h"
#include "XinputState.h"

namespace DivaHook::Input
{
	class Xinput : public IInputDevice
	{
		const float DOUBLE_TAP_THRESHOLD = 200.0f;

	public:
		static Xinput* GetInstance();
		
		bool PollInput() override; 
		bool IsDown(BYTE keycode);
		bool IsUp(BYTE keycode);
		bool IsTapped(BYTE keycode);
		bool IsReleased(BYTE keycode);
		bool IsDoubleTapped(BYTE keycode);
		bool WasDown(BYTE keycode);
		bool WasUp(BYTE keycode);

	private:
		Xinput();
		XinputState lastState;
		XinputState currentState;

		XINPUT_STATE state;

		BYTE KeyDoubleTapStates[0x3F];
		Utilities::Stopwatch KeyDoubleTapWatches[0x3F];

		static Xinput* instance;
	};
}

