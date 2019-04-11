#include "TouchPanelEmulator.h"
#include <iostream>
#include "../../Constants.h"
#include "../../Input/Mouse.h"
#include "../../Input/Keyboard.h"

using namespace DivaHook::Input;

namespace DivaHook::Components
{
	TouchPanelEmulator::TouchPanelEmulator()
	{
	}

	TouchPanelEmulator::~TouchPanelEmulator()
	{
	}

	const char* TouchPanelEmulator::GetDisplayName()
	{
		return "touch_panel_emulator";
	}

	void TouchPanelEmulator::Initialize()
	{
		state = GetTouchStatePtr((void*)TASK_TOUCH_ADDRESS);
	}

	void TouchPanelEmulator::Update()
	{
		state->ConnectionState = 1;
	}

	void TouchPanelEmulator::UpdateInput()
	{
		auto keyboard = Keyboard::GetInstance();
		auto pos = Mouse::GetInstance()->GetRelativePosition();

		state->XPosition = (float)pos.x;
		state->YPosition = (float)pos.y;

		bool touching = keyboard->IsDown(VK_LBUTTON);
		bool released = keyboard->IsReleased(VK_LBUTTON);

		state->ContactType = (touching ? 0x2 : released ? 0x1 : 0x0);
		state->Pressure = (float)(state->ContactType != 0);
	}

	TouchPanelState* TouchPanelEmulator::GetTouchStatePtr(void *address)
	{
		return (TouchPanelState*)address;
	}
}
