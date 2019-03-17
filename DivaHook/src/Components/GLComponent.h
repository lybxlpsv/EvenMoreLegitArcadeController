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
		
		double* fbAspectRatio;
	};
}

