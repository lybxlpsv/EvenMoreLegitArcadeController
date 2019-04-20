#pragma once
#include "EmulatorComponent.h"
#include "Camera.h"
#include "../Constants.h"
#include "../Input/Bindings/Binding.h"

namespace DivaHook::Components
{
	class CameraController : public EmulatorComponent
	{
	public:
		Input::Binding* ToggleBinding;

		Input::Binding* ForwardBinding;
		Input::Binding* BackwardBinding;
		Input::Binding* LeftBinding;
		Input::Binding* RightBinding;

		Input::Binding* UpBinding;
		Input::Binding* DownBinding;
		Input::Binding* FastBinding;
		Input::Binding* SlowBinding;

		Input::Binding* ClockwiseBinding;
		Input::Binding* CounterClockwiseBinding;

		Input::Binding* ZoomInBinding;
		Input::Binding* ZoomOutBinding;

		CameraController();
		~CameraController();

		bool eyeOverride = false;

		virtual const char* GetDisplayName() override;

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void UpdateInput() override;
		virtual void SetEye(bool eye);

		void SetControls(bool value);

		bool GetIsEnabled();

	private:
		void SetMouseWindowCenter();
		void SetIsEnabled(bool value);

		const float fastSpeed = 0.1f;
		const float slowSpeed = 0.0005f;
		const float normalSpeed = 0.005f;

		const float defaultRotation = 0.0f;
		const float defaultFov = 70.0f;
		const float sensitivity = 0.25f;

		float verticalRotation;
		float horizontalRotation;

		bool isEnabled;
		Camera* camera;

		uint8_t originalSetterBytes[4];
		void* cameraSetterAddresses[4] =
		{
			(void*)CAMERA_POS_SETTER_ADDRESS,
			(void*)CAMERA_INTR_SETTER_ADDRESS,
			(void*)CAMERA_ROT_SETTER_ADDRESS,
			(void*)CAMERA_PERS_SETTER_ADDRESS,
		};
	};
}
