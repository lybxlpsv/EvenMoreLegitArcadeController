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
#include "../Components/EmulatorComponent.h"
#include "../Components/Input/TouchPanelEmulator.h"
#include "../Components/Input/InputEmulator.h"

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

	static int ModuleEquip1 = 0;
	static int ModuleEquip2 = 0;
	static int BtnSeEquip = 0;
	static int Skinequip = 0;
	static bool showui = false;
	static bool showfps = false;
	static bool showui2 = false;
	static bool showabout = false;
	static int firsttime = 10000;
	static int fps_limit = 0;
	static int fps_limit_set = 0;
	static int sfx_volume = 100;
	static int bgm_volume = 100;
	static float ui_transparency = 0.8;
	static float sleep = 0;
	static float fpsdiff = 0;
	static bool MorphologicalAA = 0;
	static bool MorphologicalAA2 = 0;
	static bool TemporalAA = 0;

	static bool ToonShader = true;
	static bool ToonShader2 = false;

	PlayerDataManager* pdm;
	FrameRateManager* frm;

	static int maxrenderwidth = 2560;
	static int maxrenderheight = 1440;
	static std::chrono::time_point m_BeginFrame = system_clock::now();
	static std::chrono::time_point prev_time_in_seconds = time_point_cast<seconds>(m_BeginFrame);
	static unsigned frame_count_per_second = 0;
	
	GLComponent::GLComponent()
	{
		pdm = new PlayerDataManager();
		frm = new FrameRateManager();
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
			if (showui) { showui = false; showui2 = false; }
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

		bool p_open = true;
		RECT hWindow;
		GetClientRect(DivaHook::MainModule::DivaWindowHandle, &hWindow);

		ImGui::SetNextWindowBgAlpha(ui_transparency);
		
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplOpenGL2_NewFrame();
		ImGui::NewFrame();

		if (firsttime > 0)
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

		if (showfps)
		{
			ImGui::SetNextWindowBgAlpha(ui_transparency);
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

		if (showabout)
		{
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::Begin("About DivaHook/ELAC", &showabout, window_flags);
			ImGui::Text("Main Developer:");
			ImGui::Text("Samyuu");
			ImGui::Text("DIVAHook/ELAC Contributors:");
			ImGui::Text("Brolijah, Crash5band, Rakisaionji, Deathride58, lybxlpsv");
			ImGui::Text("DIVAHook UI by:");
			ImGui::Text("lybxlpsv");
			ImGui::Text("DIVAHook UI Contributors:");
			ImGui::Text("BesuBaru");
			if (ImGui::Button("Close")) { showabout = false; };
			ImGui::End();
		}

		if (showui) {
			ImGui::SetNextWindowBgAlpha(ui_transparency);
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			DivaHook::Components::TouchPanelEmulator::HookWindowFocus();
			DivaHook::Components::InputEmulator::HookWindowFocus();
			ImGui::Begin("DivaHook Config", &showui, window_flags);
			if (ImGui::CollapsingHeader("Modules and Custom Skins/Sounds"))
			{
				ImGui::Text("--- Changes only takes effect after entering a new stage. ---");
				ImGui::InputInt("Module 1 ID", &ModuleEquip1);
				ImGui::InputInt("Module 2 ID", &ModuleEquip2);
				ImGui::InputInt("Button SFX ID", &BtnSeEquip);
				ImGui::InputInt("HUD Skin ID", &Skinequip);
			}
			if (ImGui::CollapsingHeader("Internal Resolution")) 
			{
				ImGui::SliderInt("Resolution Width", fbWidth, 640, maxrenderwidth);
				ImGui::SliderInt("Resolution Height", fbHeight, 360, maxrenderheight);
			}
			if (ImGui::CollapsingHeader("Framerate"))
			{
				ImGui::InputInt("Framerate Cap", &fps_limit_set);
			}
			if (ImGui::CollapsingHeader("Graphics settings"))
			{
				ImGui::Text("--- Anti-Aliasing ---");
				ImGui::Checkbox("MLAA (Morphological AA)", &MorphologicalAA);
				ImGui::Checkbox("TAA (Temporal AA)", &TemporalAA);
				ImGui::Text("--- Bug Fixes ---");
				ImGui::Checkbox("Toon Shader (When Running with: -hdtv1080/-aa)", &ToonShader);
			}
			if (ImGui::CollapsingHeader("Sound Settings"))
			{
				ImGui::SliderInt("HP Volume", &bgm_volume, 0, 100);
				ImGui::SliderInt("ACT Volume", &sfx_volume, 0, 100);
			}
			if (ImGui::CollapsingHeader("UI Settings"))
			{
				ImGui::SliderFloat("UI Transparency", &ui_transparency, 0, 1.0);
				ImGui::Checkbox("Framerate Overlay", &showfps);
			}
			if (ImGui::Button("Save")) {} ImGui::SameLine();
			if (ImGui::Button("Close")) { showui = false; }; ImGui::SameLine();
			if (ImGui::Button("About")) { showabout = true; } ImGui::SameLine();
			ImGui::End();
		}
		else
		{
			DivaHook::Components::TouchPanelEmulator::HookWindowUnFocus();
			DivaHook::Components::InputEmulator::HookWindowUnFocus();
		}
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		//The worst framelimit/pacer ever.
		//TODO : Destroy this thing and replace with a much better one.

		if (fps_limit_set != fps_limit)
		{
			m_BeginFrame = system_clock::now();
			prev_time_in_seconds = time_point_cast<seconds>(m_BeginFrame);
			frame_count_per_second = 0;
			fps_limit = fps_limit_set;
		}

		auto invFpsLimit = round<system_clock::duration>(dsec{ 1. / fps_limit });
		auto m_EndFrame = m_BeginFrame + invFpsLimit;

		auto time_in_seconds = time_point_cast<seconds>(system_clock::now());
		++frame_count_per_second;
		if (time_in_seconds > prev_time_in_seconds)
		{
			frame_count_per_second = 0;
			prev_time_in_seconds = time_in_seconds;
		}

		// This part keeps the frame rate.
		if (fps_limit > 30)
			std::this_thread::sleep_until(m_EndFrame);
		m_BeginFrame = m_EndFrame;
		m_EndFrame = m_BeginFrame + invFpsLimit;

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

		int* fbWidth = (int*)FB_RESOLUTION_WIDTH_ADDRESS;
		int* fbHeight = (int*)FB_RESOLUTION_HEIGHT_ADDRESS;

		maxrenderheight = *fbHeight;
		maxrenderwidth = *fbWidth;

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
		frm->fps_limit = fps_limit;
		if (fps_limit > 30)
		{
			frm->useFpsLimitValue = true;
		}
		else frm->useFpsLimitValue = false;

		int* taa;
		taa = (int*)GFX_TEMPORAL_AA;
		if (TemporalAA)
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

		if (MorphologicalAA)
		{
			if (!MorphologicalAA2) {
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x006EE6F8, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x006EE6F8 + 0) = 0x85;
				*((byte*)0x006EE6F8 + 1) = 0x45;
				*((byte*)0x006EE6F8 + 2) = 0x10;
				VirtualProtect((BYTE*)0x006EE6F8, 3, oldProtect, &bck);
				MorphologicalAA2 = true;
			}
		}
		else {
			if (MorphologicalAA2) {
				DWORD oldProtect, bck;
				VirtualProtect((byte*)0x006EE6F8, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x006EE6F8 + 0) = 0x83;
				*((byte*)0x006EE6F8 + 1) = 0xE0;
				*((byte*)0x006EE6F8 + 2) = 0x00;
				VirtualProtect((byte*)0x006EE6F8, 3, oldProtect, &bck);
				MorphologicalAA2 = !MorphologicalAA2;
			}
		}

		if (ToonShader)
		{
			if (!ToonShader2)
			{
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00715B86, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00715B86 + 0) = 0x90;
				*((byte*)0x00715B86 + 1) = 0x90;
				VirtualProtect((BYTE*)0x006EE6F8, 2, oldProtect, &bck);
				ToonShader2 = true;
			}
		}
		else {
			if (ToonShader2)
			{
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00715B86, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00715B86 + 0) = 0x74;
				*((byte*)0x00715B86 + 1) = 0x0d;
				VirtualProtect((BYTE*)0x006EE6F8, 2, oldProtect, &bck);
				ToonShader2 = !ToonShader2;
			}
		}
		pdm->customPlayerData->ModuleEquip[0] = ModuleEquip1;
		pdm->customPlayerData->ModuleEquip[1] = ModuleEquip2;
		pdm->customPlayerData->BtnSeEquip = BtnSeEquip;
		pdm->customPlayerData->SkinEquip = Skinequip;
		if (showui) {
			if (!showui2)
			{
				bgm_volume = pdm->playerData->hp_vol;
				sfx_volume = pdm->playerData->act_vol;
				showui2 = true;
			}
			pdm->playerData->hp_vol = bgm_volume;
			pdm->playerData->act_vol = sfx_volume;
		}
		pdm->Update();
		frm->Update();
		if (firsttime > 0) firsttime = firsttime - round(GetElapsedTime());
		return;
	}

	void GLComponent::UpdateInput()
	{
		pdm->UpdateInput();
		frm->UpdateInput();
	}
}
