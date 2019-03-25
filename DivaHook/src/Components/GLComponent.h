#pragma once
#include "EmulatorComponent.h"
#include <string>

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

		std::string inputString;
	};
}

