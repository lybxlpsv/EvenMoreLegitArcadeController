#include <iostream>
#include <vector>
#include "windows.h"
#include "Constants.h"
#include "MainModule.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"
#include "Components/EmulatorComponent.h"
#include "Components/InputEmulator.h"
#include "Components/TouchPanelEmulator.h"
#include "Components/SysTimer.h"
#include "Components/DebugComponent.h"
#include "Utilities/Stopwatch.h"

using namespace DivaHook::Components;
using namespace DivaHook::Utilities;

namespace DivaHook
{
	const DWORD JMP_HOOK_SIZE = 0x5;
	const BYTE JMP_OPCODE = 0xE9;
	const BYTE NOP_OPCODE = 0x90;

	const DWORD HookByteLength = 0x5;
	const DWORD ReturnAddress = ENGINE_UPDATE_HOOK_TARGET_ADDRESS + HookByteLength;

	float elpasedTime;
	bool hasWindowFocus, hadWindowFocus;
	bool firstUpdateTick = true;

	Stopwatch updateStopwatch;
	std::vector<EmulatorComponent*> components;

	void InstallHook(BYTE *address, DWORD overrideAddress, DWORD length)
	{
		DWORD oldProtect, dwBkup, dwRelAddr;

		VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtect);

		// calculate the distance between our address and our target location
		// and subtract the 5bytes, which is the size of the jmp
		// (0xE9 0xAA 0xBB 0xCC 0xDD) = 5 bytes
		dwRelAddr = (DWORD)(overrideAddress - (DWORD)address) - JMP_HOOK_SIZE;

		*address = JMP_OPCODE;

		// overwrite the next 4 bytes (which is the size of a DWORD)
		// with the dwRelAddr
		*((DWORD *)(address + 1)) = dwRelAddr;

		// overwrite the remaining bytes with the NOP opcode (0x90)
		for (DWORD x = JMP_HOOK_SIZE; x < length; x++)
			*(address + x) = NOP_OPCODE;

		VirtualProtect(address, length, oldProtect, &dwBkup);
	}

	void InitializeTick()
	{
		//printf("InitTick(): base init\n");

		components.push_back(new InputEmulator());
		components.push_back(new TouchPanelEmulator());
		components.push_back(new SysTimer());

#ifdef _DEBUG
		components.push_back(new DebugComponent());
#endif

		MainModule::DivaWindowHandle = FindWindow(0, MainModule::DivaWindowName);

		if (MainModule::DivaWindowHandle == NULL)
			MainModule::DivaWindowHandle = FindWindow(0, MainModule::GlutDefaultName);

		updateStopwatch.Start();
	}

	void UpdateTick()
	{
		if (firstUpdateTick)
		{
			InitializeTick();
			firstUpdateTick = false;

			for (auto& component : components)
				component->Initialize();
		}

		elpasedTime = updateStopwatch.Restart();

		for (auto& component : components)
			component->Update();

		hadWindowFocus = hasWindowFocus;
		hasWindowFocus = MainModule::DivaWindowHandle == NULL || GetForegroundWindow() == MainModule::DivaWindowHandle;

		if (hasWindowFocus)
		{
			Input::Keyboard::GetInstance()->PollInput();
			Input::Mouse::GetInstance()->PollInput();

			for (auto& component : components)
				component->UpdateInput();
		}

		if (hasWindowFocus && !hadWindowFocus)
		{
			for (auto& component : components)
				component->OnFocusGain();
		}

		if (!hasWindowFocus && hadWindowFocus)
		{
			for (auto& component : components)
				component->OnFocusLost();
		}
	}

	void _declspec(naked) FunctionHook()
	{
		_asm
		{
			call UpdateTick
			jmp[ReturnAddress]
		}
	}

	void InitializeHooks()
	{
		InstallHook((BYTE*)ENGINE_UPDATE_HOOK_TARGET_ADDRESS, (DWORD)FunctionHook, HookByteLength);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DivaHook::InitializeHooks();
		DivaHook::MainModule::Module = hModule;
		break;
	}

	return TRUE;
}

