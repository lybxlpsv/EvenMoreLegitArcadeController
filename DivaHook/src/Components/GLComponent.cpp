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
#include "FrameRateManager.h"
#include "Input/InputEmulator.h"
#include "Input/TouchPanelEmulator.h"
#include "../Components/EmulatorComponent.h"

#include <chrono>
#include <thread>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"
#include "GL/gl.h"

namespace DivaHook::Components
{
	using namespace std::chrono;
	using dsec = duration<double>;

	static int moduleEquip1 = 0;
	static int moduleEquip2 = 0;
	static int btnSeEquip = 0;
	static int skinEquip = 0;
	static bool showUi = false;
	static bool showFps = false;
	static bool showUi2 = false;
	static bool showAbout = false;
	static int firstTime = 10000;
	static int fpsLimit = 0;
	static int fpsLimitSet = 0;
	static int sfxVolume = 100;
	static int bgmVolume = 100;
	static float uiTransparency = 0.8;
	static float sleep = 0;
	static float fpsDiff = 0;
	static bool morphologicalAA = 0;
	static bool morphologicalAA2 = 0;
	static bool temporalAA = 0;

	static bool toonShader = true;
	static bool toonShader2 = false;

	PlayerDataManager* pdm;
	FrameRateManager* frm;
	InputEmulator* inp;
	TouchPanelEmulator* tch;

	static int maxRenderWidth = 2560;
	static int maxRenderHeight = 1440;
	static std::chrono::time_point mBeginFrame = system_clock::now();
	static std::chrono::time_point prevTimeInSeconds = time_point_cast<seconds>(mBeginFrame);
	static unsigned frameCountPerSecond = 0;
	
	GLComponent::GLComponent()
	{
		pdm = new PlayerDataManager();
		frm = new FrameRateManager();
		inp = new InputEmulator();
		tch = new TouchPanelEmulator();
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
			if (showUi) { showUi = false; showUi2 = false; }
			else showUi = true;
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

		bool p_open = true;
		RECT hWindow;
		GetClientRect(DivaHook::MainModule::DivaWindowHandle, &hWindow);

		ImGui::SetNextWindowBgAlpha(uiTransparency);
		
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplOpenGL2_NewFrame();
		ImGui::NewFrame();

		if (firstTime > 0)
		{
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::Begin("ELAC", &p_open, window_flags);
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::Text("Press Ctrl+LShift+Backspace to show/hide UI.");
			ImGui::End();
		}

		if (showFps)
		{
			ImGui::SetNextWindowBgAlpha(uiTransparency);
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::Begin("FPS", &p_open, window_flags);
			ImGui::SetWindowPos(ImVec2(hWindow.right - 100, 0));
			ImGui::SetWindowSize(ImVec2(100, 70));
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			ImGui::Text("FT: %.2fms", 1000 / ImGui::GetIO().Framerate);
			ImGui::End();
		}

		if (showAbout)
		{
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::Begin("About DivaHook/ELAC", &showAbout, window_flags);
			ImGui::Text("Main Developer:");
			ImGui::Text("Samyuu");
			ImGui::Text("DIVAHook/ELAC Contributors:");
			ImGui::Text("Brolijah, Crash5band, Rakisaionji, Deathride58, lybxlpsv");
			ImGui::Text("DIVAHook UI by:");
			ImGui::Text("lybxlpsv");
			ImGui::Text("DIVAHook UI Contributors:");
			ImGui::Text("BesuBaru");
			if (ImGui::Button("Close")) { showAbout = false; };
			ImGui::End();
		}

		if (showUi) {
			ImGui::SetNextWindowBgAlpha(uiTransparency);
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::Begin("DivaHook Config", &showUi, window_flags);
			if (ImGui::CollapsingHeader("Modules and Custom Skins/Sounds"))
			{
				ImGui::Text("--- Changes only takes effect after entering a new stage. ---");
				ImGui::InputInt("Module 1 ID", &moduleEquip1);
				ImGui::InputInt("Module 2 ID", &moduleEquip2);
				ImGui::InputInt("Button SFX ID", &btnSeEquip);
				ImGui::InputInt("HUD Skin ID", &skinEquip);
			}
			if (ImGui::CollapsingHeader("Internal Resolution")) 
			{
				ImGui::SliderInt("Resolution Width", fbWidth, 640, maxRenderWidth);
				ImGui::SliderInt("Resolution Height", fbHeight, 360, maxRenderHeight);
			}
			if (ImGui::CollapsingHeader("Framerate"))
			{
				ImGui::Text("--- Set the FPS cap to 0 only if you have vsync. ---");
				ImGui::InputInt("Framerate Cap", &fpsLimitSet);
			}
			if (ImGui::CollapsingHeader("Graphics settings"))
			{
				ImGui::Text("--- Anti-Aliasing ---");
				ImGui::Checkbox("MLAA (Morphological AA)", &morphologicalAA);
				ImGui::Checkbox("TAA (Temporal AA)", &temporalAA);
				ImGui::Text("--- Bug Fixes ---");
				ImGui::Checkbox("Toon Shader (When Running with: -hdtv1080/-aa)", &toonShader);
			}
			if (ImGui::CollapsingHeader("Sound Settings"))
			{
				ImGui::SliderInt("HP Volume", &bgmVolume, 0, 100);
				ImGui::SliderInt("ACT Volume", &sfxVolume, 0, 100);
			}
			if (ImGui::CollapsingHeader("UI Settings"))
			{
				ImGui::SliderFloat("UI Transparency", &uiTransparency, 0, 1.0);
				ImGui::Checkbox("Framerate Overlay", &showFps);
			}
			if (ImGui::Button("Close")) { showUi = false; }; ImGui::SameLine();
			if (ImGui::Button("About")) { showAbout = true; } ImGui::SameLine();
			ImGui::End();
		}
		else
		{

		}
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		//The worst framelimit/pacer ever.
		//TODO : Destroy this thing and replace with a much better one.

		if (fpsLimitSet != fpsLimit)
		{
			mBeginFrame = system_clock::now();
			prevTimeInSeconds = time_point_cast<seconds>(mBeginFrame);
			frameCountPerSecond = 0;
			fpsLimit = fpsLimitSet;
		}

		auto invFpsLimit = round<system_clock::duration>(dsec{ 1. / fpsLimit });
		auto mEndFrame = mBeginFrame + invFpsLimit;

		auto timeInSeconds = time_point_cast<seconds>(system_clock::now());
		++frameCountPerSecond;
		if (timeInSeconds > prevTimeInSeconds)
		{
			frameCountPerSecond = 0;
			prevTimeInSeconds = timeInSeconds;
		}

		// This part keeps the frame rate.
		if (fpsLimit > 19)
			std::this_thread::sleep_until(mEndFrame);
		mBeginFrame = mEndFrame;
		mEndFrame = mBeginFrame + invFpsLimit;

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
		frm->Initialize();
		tch->Initialize();
		inp->Initialize();

		moduleEquip1 = pdm->customPlayerData->ModuleEquip[0];
		moduleEquip2 = pdm->customPlayerData->ModuleEquip[1];
		btnSeEquip = pdm->customPlayerData->BtnSeEquip;
		skinEquip = pdm->customPlayerData->SkinEquip;

		int* fbWidth = (int*)FB_RESOLUTION_WIDTH_ADDRESS;
		int* fbHeight = (int*)FB_RESOLUTION_HEIGHT_ADDRESS;

		maxRenderHeight = *fbHeight;
		maxRenderWidth = *fbWidth;

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
		frm->fpsLimit = fpsLimit;
		if (fpsLimit > 19)
		{
			frm->useFpsLimitValue = true;
		}
		else frm->useFpsLimitValue = false;
				
		int* taa;
		taa = (int*)GFX_TEMPORAL_AA;
		if (temporalAA)
		{
			DWORD oldProtect, bck;
			VirtualProtect((BYTE*)GFX_TEMPORAL_AA, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			Memory::Write(GFX_TEMPORAL_AA, 0x01);
			VirtualProtect((BYTE*)GFX_TEMPORAL_AA, 1, oldProtect, &bck);
		}
		else {
			DWORD oldProtect, bck;
			VirtualProtect((BYTE*)GFX_TEMPORAL_AA, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			Memory::Write(GFX_TEMPORAL_AA, 0x00);
			VirtualProtect((BYTE*)GFX_TEMPORAL_AA, 1, oldProtect, &bck);
		}

		if (morphologicalAA)
		{
			if (!morphologicalAA2) {
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x006EE6F8, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x006EE6F8 + 0) = 0x85;
				*((byte*)0x006EE6F8 + 1) = 0x45;
				*((byte*)0x006EE6F8 + 2) = 0x10;
				VirtualProtect((BYTE*)0x006EE6F8, 3, oldProtect, &bck);
				morphologicalAA2 = true;
			}
		}
		else {
			if (morphologicalAA2) {
				DWORD oldProtect, bck;
				VirtualProtect((byte*)0x006EE6F8, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x006EE6F8 + 0) = 0x83;
				*((byte*)0x006EE6F8 + 1) = 0xE0;
				*((byte*)0x006EE6F8 + 2) = 0x00;
				VirtualProtect((byte*)0x006EE6F8, 3, oldProtect, &bck);
				morphologicalAA2 = !morphologicalAA2;
			}
		}

		if (toonShader)
		{
			if (!toonShader2)
			{
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00715B86, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00715B86 + 0) = 0x90;
				*((byte*)0x00715B86 + 1) = 0x90;
				VirtualProtect((BYTE*)0x006EE6F8, 2, oldProtect, &bck);
				toonShader2 = true;
			}
		}
		else {
			if (toonShader2)
			{
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00715B86, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00715B86 + 0) = 0x74;
				*((byte*)0x00715B86 + 1) = 0x0d;
				VirtualProtect((BYTE*)0x006EE6F8, 2, oldProtect, &bck);
				toonShader2 = !toonShader2;
			}
		}
		pdm->customPlayerData->ModuleEquip[0] = moduleEquip1;
		pdm->customPlayerData->ModuleEquip[1] = moduleEquip2;
		pdm->customPlayerData->BtnSeEquip = btnSeEquip;
		pdm->customPlayerData->SkinEquip = skinEquip;
		if (showUi) {
			if (!showUi2)
			{
				bgmVolume = pdm->playerData->hp_vol;
				sfxVolume = pdm->playerData->act_vol;
				showUi2 = true;
			}
			pdm->playerData->hp_vol = bgmVolume;
			pdm->playerData->act_vol = sfxVolume;
		}
		pdm->Update();
		frm->Update();

		if (!showUi)
		{
			tch->Update();
			inp->Update();
		}

		if (firstTime > 0) firstTime = firstTime - round(GetElapsedTime());
		return;
	}

	void GLComponent::UpdateInput()
	{
		pdm->UpdateInput();
		frm->UpdateInput();
		if (!showUi)
		{
			inp->UpdateInput();
			tch->UpdateInput();
		}
	}
}
