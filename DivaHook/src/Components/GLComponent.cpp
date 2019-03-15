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
	static int firsttime = 5000;
	
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

		if (showui) {
			ImGui::Begin("DIVA");
			ImGui::Text("Changes only takes effect after entering a new stage.");
			ImGui::InputInt("ModuleEquip1", &ModuleEquip1);
			ImGui::InputInt("ModuleEquip2", &ModuleEquip2);
			ImGui::InputInt("btnSeEquip", &BtnSeEquip);
			ImGui::InputInt("SkinEquip", &Skinequip);
			ImGui::InputInt("RenderResWidth", fbWidth);
			ImGui::InputInt("RenderResHeight", fbHeight);
			ImGui::End();
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
