#pragma once

namespace DivaHook::Components
{
	struct TouchPanelState
	{
		int Padding00[0x1A];
		int ConnectionState;
		int Padding01[0x06];
		float XPosition;
		float YPosition;
		float Pressure;
		int ContactType;
	};
}
