#include "GLComponent.h"
#include "GLMemory.h"
#include <iostream>
#include <Windows.h>
#include "Input/InputState.h"
#include "GameState.h"
#include "PlayerData.h"
#include "../Input/Keyboard.h"
#include "../Constants.h"
#include "wtypes.h"
#include "../MainModule.h"
#include "CustomPlayerData.h"
#include "PlayerDataManager.h"
#include "../Components/EmulatorComponent.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"
#include "GL/gl.h"

namespace DivaHook::Components
{
	static int ModuleEquip1 = 0;
	static int ModuleEquip2 = 0;
	static int BtnSeEquip = 0;
	static int Skinequip = 0;
	static bool showui = false;
	static bool showfps = true;
	static int firsttime = 5000;
	static int fps_limit = 0;
	static int sfx_volume = 100;
	static int bgm_volume = 100;
	static int ui_transparency = 50;
	
	PlayerDataManager* pdm;

	GLComponent::GLComponent()
	{
		pdm = new PlayerDataManager();
	}

	GLComponent::~GLComponent()
	{

	}

	void hwglSwapBuffers(_In_ HDC hDc)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto keyboard = DivaHook::Input::Keyboard::GetInstance();
		io.MouseDown[0] = keyboard->IsDown(VK_LBUTTON);
		io.MouseDown[1] = keyboard->IsDown(VK_RBUTTON);
		io.MouseDown[2] = keyboard->IsDown(VK_MBUTTON);

		io.KeysDown[ImGuiKey_Enter] = keyboard->IsDown(VK_RETURN);
		io.KeysDown[ImGuiKey_Backspace] = keyboard->IsDown(VK_BACK);

		int* fbWidth = (int*)FB_RESOLUTION_WIDTH_ADDRESS;
		int* fbHeight = (int*)FB_RESOLUTION_HEIGHT_ADDRESS;

		if ((keyboard->IsDown(VK_CONTROL)) && (keyboard->IsDown(VK_LSHIFT)) && (keyboard->IsTapped(VK_BACK)))
		{
			if (showui) showui = false;
			else showui = true;
		}

		int i = 48;
		while (i != 58)
		{
			if (keyboard->IsTapped(i))
			{
				io.AddInputCharacter(i);
			}
			i++;
		}
		
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplOpenGL2_NewFrame();
		ImGui::NewFrame();

		if (firsttime > 0)
		{
			ImGui::Begin("ELAC");
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::Text("Press Ctrl+LShift+Backspace to show/hide UI.");
			ImGui::End();
		}

		if (showfps)
		{
			ImGui::Begin("FPS/ms");
			ImGui::Text("Framerate: %.1FPS", ImGui::GetIO().Framerate);
			ImGui::Text("Frametime: %.3fms", 1000.0f / ImGui::GetIO().Framerate);
			ImGui::SetWindowPos(ImVec2(1000, 0));
		}

		if (showui) {
			ImGui::Begin("DivaHook Config");
			ImGui::Text("Changes only takes effect after entering a new stage.");
			ImGui::Text("--- Modules and Custom Skins/Sounds ---");
			ImGui::InputInt("Module 1 ID", &ModuleEquip1);
			ImGui::InputInt("Module 2 ID", &ModuleEquip2);
			ImGui::InputInt("Button SFX ID", &BtnSeEquip);
			ImGui::InputInt("HUD Skin ID", &Skinequip);
			ImGui::Text("--- Internal Resolution ---");
			ImGui::SliderInt("Resolution Width", fbWidth, 640, 2560);
			ImGui::SliderInt("Resolution Height", fbHeight, 360, 1440);
			ImGui::Text("--- Framerate ---");
			ImGui::InputInt("Framerate Cap", &fps_limit);
			ImGui::Text("--- Sound Settings ---");
			ImGui::SliderInt("BGM Volume", &bgm_volume, 0, 100);
			ImGui::SliderInt("SFX Volume", &sfx_volume, 0, 100);
			ImGui::Text("--- UI Settings ---");
			ImGui::SliderInt("UI Transparency", &ui_transparency, 0, 100);
			ImGui::Checkbox("Framerate Overlay", &showfps);
			if (ImGui::Button("Save"))
			{
				// do stuff
			}
			ImGui::End();
		}
		else
		{

		}
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		return;
	}

	DWORD owglSwapBuffers = 0;
	void __declspec(naked) SwapTrampoline()
	{
		__asm {
			PUSHFD										//Stores EFLAGS
			PUSHAD										//Stores GP Registers
			CALL hwglSwapBuffers						//Redirects the execution to our function
			POPAD                                       //Restores GP Registers
			POPFD                                       //Restores EFLAGS
			PUSH EBP									//Restores Overwritten PUSH EBP
			MOV EBP, ESP								//Restores Overwritten MOV  EBP, ESP
			JMP[owglSwapBuffers]						//Restores the execution to the original function
		}
	}

	const char* GLComponent::GetDisplayName()
	{
		return "gl_component";
	}

	void GLComponent::Initialize()
	{
		pdm->Initialize();
		ModuleEquip1 = pdm->customPlayerData->ModuleEquip[0];
		ModuleEquip2 = pdm->customPlayerData->ModuleEquip[1];
		BtnSeEquip = pdm->customPlayerData->BtnSeEquip;
		Skinequip = pdm->customPlayerData->SkinEquip;

		DWORD AddressToHook = (DWORD)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");
		owglSwapBuffers = Memory::JumpHook(AddressToHook, (DWORD)SwapTrampoline, 5);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.WantCaptureKeyboard == true;

		ImGui_ImplWin32_Init(MainModule::DivaWindowHandle);
		ImGui_ImplOpenGL2_Init();
		ImGui::StyleColorsDark();
		
		printf("GLComponent::Initialize(): Initialized\n");
	}
	
	void GLComponent::Update()
	{
		pdm->customPlayerData->ModuleEquip[0] = ModuleEquip1;
		pdm->customPlayerData->ModuleEquip[1] = ModuleEquip2;
		pdm->customPlayerData->BtnSeEquip = BtnSeEquip;
		pdm->customPlayerData->SkinEquip = Skinequip;
		pdm->Update();
		if (firsttime > 0) firsttime = firsttime - round(GetElapsedTime());
		//fps = 1000 / GetElapsedTime(); this takes alot of cpu :(
		return;
	}

	void GLComponent::UpdateInput()
	{
		pdm->UpdateInput();
	}
}
