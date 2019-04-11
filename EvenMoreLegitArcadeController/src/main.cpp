#include "Injection/DllInjector.h"

const char* ELAC_VERSION = "4.10.00";

const std::string DIVA_PROCESS_NAME = "diva.exe";
const std::string DIVA_HOOK_DLL_NAME = "divahook.dll";

char waitDisplayStrings[8][6] =
{
	"o....",
	".o...",
	"..o..",
	"...o.",
	"....o",
	"...o.",
	"..o..",
	".o...",
};

int GetDirectorySeperatorPosition(std::string path)
{
	for (int i = path.size() - 1; i >= 0; i--)
	{
		auto currentChar = path[i];
		if (currentChar == '\\' || currentChar == '/')
			return i;
	}

	return -1;
}

std::string GetModuleDirectory()
{
	HMODULE module = GetModuleHandleW(NULL);
	CHAR modulePathBuffer[MAX_PATH];
	GetModuleFileName(module, modulePathBuffer, MAX_PATH);

	auto modulePath = std::string(modulePathBuffer);
	int seperatorPos = GetDirectorySeperatorPosition(modulePath);

	if (seperatorPos != -1)
		return std::string(modulePathBuffer).substr(0, seperatorPos);

	return NULL;
}

void PrintProgramInfo()
{
	printf("// -------------------------------------------------\n");

#ifdef _DEBUG
	printf("// -- DEBUG_BUILD: --\n//\n");
#endif

	printf("// Even More Legit Arcade Controller (ELAC) (Fork)	\n");
	printf("// !UNOFFICIAL VERSION! DO NOT ASK ORIGINAL			\n");
	printf("//       MAINTAINER/samyuu FOR SUPPORT!				\n");
	printf("// Git Repo : https://git.io/fjftz					\n");
	printf("// v.%s                            -by samyuu       \n", ELAC_VERSION);
	printf("// -------------------------------------------------\n");
}

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi);
	return cbsi.dwCursorPosition;
}

void WaitExit(int duration = 1000, int iterations = 32 + 1)
{
	DWORD interval = duration / iterations;

	auto consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(consoleHandle, &cursorInfo);

	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
	{
		auto prerPos = GetConsoleCursorPosition(consoleHandle);

		for (int i = 0; i < iterations; i++)
		{
			SetConsoleCursorPosition(consoleHandle, prerPos);
			printf("%s", waitDisplayStrings[i % 8]);
			Sleep(interval);
		}
	}
	cursorInfo.bVisible = true;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

bool DoesFileExist(std::string filePath)
{
	auto fileAttrib = GetFileAttributes(filePath.c_str());
	return fileAttrib != INVALID_FILE_ATTRIBUTES;
}

HANDLE Startpausedprocess(char* cmd, PHANDLE ptr_thread) // cleaned up a bit, but no RAII
{
	if (ptr_thread == nullptr) return nullptr;

	PROCESS_INFORMATION pi;
	STARTUPINFOA si{}; // initialize with zeroes.
	si.cb = sizeof(STARTUPINFOA);

	if (!CreateProcessA(nullptr, cmd, nullptr, nullptr, false, CREATE_SUSPENDED,
		nullptr, nullptr, std::addressof(si), std::addressof(pi)))
	{
		std::cerr << "CreateProcess failed, " << GetLastError() << '\n';
		*ptr_thread = nullptr;
		return nullptr;
	}

	*ptr_thread = pi.hThread;
	return pi.hProcess;
}

int main(int argc, char** argv)
{
	PrintProgramInfo();

	auto moduleDirectory = GetModuleDirectory();
	auto dllPath = moduleDirectory + "/" + DIVA_HOOK_DLL_NAME;

	if (!DoesFileExist(dllPath))
	{
		printf("main(): Unable to locate %s\n", DIVA_HOOK_DLL_NAME.c_str());
		printf("main(): Press enter to exit...");
		std::cin.get();
		return EXIT_FAILURE;
	}
	else
	{
		printf("main(): %s successfully located\n", DIVA_HOOK_DLL_NAME.c_str());
	}
	
	Injection::DllInjector injector;
	bool result = injector.InjectDll(DIVA_PROCESS_NAME, dllPath);
	
	if (!result)
	{
		printf("main(): Injection failed. Launching...\n");
		
		char buffer[256];
		for (int i = 1; i < argc; ++i)
		{
			if (i == 1)
				strncpy_s(buffer, argv[i], sizeof(buffer));
			else strncat_s(buffer, argv[i], sizeof(buffer));
		}
		printf(buffer);
		HANDLE thread = nullptr;
		auto process = Startpausedprocess(buffer, std::addressof(thread));
		SetPriorityClass(process, 0x00000080);
		bool result = injector.InjectDll(DIVA_PROCESS_NAME, dllPath);
		if (!result)
		{
			printf("main(): Injection failed. Exiting...");
			return EXIT_FAILURE;
		}

		printf("main(): Exiting ");

		WaitExit();

		ResumeThread(thread);

		CloseHandle(thread);
		CloseHandle(process);
	}
	else {
		printf("main(): Exiting ");

		WaitExit();
	}

	
	return EXIT_SUCCESS;
}
