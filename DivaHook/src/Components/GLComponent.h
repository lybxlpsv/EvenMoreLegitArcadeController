#pragma once
#include "EmulatorComponent.h"

namespace DivaHook::Components
{
	class GLComponent : public EmulatorComponent
	{
	public:
		GLComponent();
		~GLComponent();

		virtual const char* GetDisplayName() override;

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void UpdateInput() override;

		float* uiAspectRatio;
		float* uiWidth;
		float* uiHeight;
		
		int* fb1Width;
		int* fb1Height;
		int* fb2Width;
		int* fb2Height;

		bool showgui = false;

		double* fbAspectRatio;
	};
}

