#include <windows.h>
#include <cstdio>
#include <stdio.h>
#include <string>

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

static DWORD pDirect3DInitFn = 0x004400a2;
static DWORD pScreenWidth = 0x006d196c;
static DWORD pScreenHeight = 0x006d1968;

const char* SECTION = "Screen";
const char* WIDTH_KEY = "Width";
const char* HEIGHT_KEY = "Height";

int windowWidth = DEFAULT_WINDOW_WIDTH;
int windowHeight = DEFAULT_WINDOW_HEIGHT;

std::string GetConfigPath() {
	CHAR fullPath[MAX_PATH];
	GetModuleFileNameA(nullptr, fullPath, MAX_PATH);

	std::string path(fullPath);
	size_t pos = path.find_last_of("\\/");
	if (pos != std::string::npos) {
		path = path.substr(0, pos + 1);
	}
	else {
		path = ".\\";
	}

	return path + "hook.cfg";
}

bool ReadIntFromConfig(const char* section, const char* key, int& value) {
	char buffer[32];
	GetPrivateProfileStringA(section, key, "", buffer, sizeof(buffer), GetConfigPath().c_str());

	if (strlen(buffer) == 0) {
		return false;
	}

	try {
		value = std::stoi(buffer);
	}
	catch (...) {
		return false;
	}

	return true;
}

void WriteIntToConfig(const char* section, const char* key, int value) {
	char buffer[32];
	sprintf_s(buffer, "%d", value);
	WritePrivateProfileStringA(section, key, buffer, GetConfigPath().c_str());
}

int ReadOrDefaultIntFromConfig(const char* section, const char* key, int defaultValue) {
	int value;
	bool success = ReadIntFromConfig(section, key, value);

	if (!success) {
		value = defaultValue;
		WriteIntToConfig(section, key, defaultValue);
	}

	return value;
}

static __declspec(naked) void Direct3DInitHook(void) {
    __asm {
        NOP
		NOP
		NOP
		NOP
		NOP
		NOP
    }

    // MessageBox(NULL, "Hello", "Seuko", 0);
	*(int*)pScreenWidth = windowWidth;
	*(int*)pScreenHeight = windowHeight;

	__asm {
		MOV EAX, pDirect3DInitFn
		add eax, 5
		JMP EAX
	}
}

BOOL PrimitiveHookFunction(const DWORD originalFn, DWORD hookFn, size_t copyBytes) {
	DWORD OldProtection = { 0 };
	BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)originalFn, copyBytes, PAGE_EXECUTE_READWRITE, &OldProtection);
	if (!success) {
		DWORD error = GetLastError();
		return 0;
	}

	*(BYTE*)((LPBYTE)originalFn) = 0xE9; //JMP FAR
	DWORD offset = (((DWORD)hookFn) - ((DWORD)originalFn + 5)); //Offset math.
	*(DWORD*)((LPBYTE)originalFn + 1) = offset;

	for (size_t i = 5; i < copyBytes; i++) {
		*(BYTE*)((LPBYTE)originalFn + i) = 0x90; //JMP FAR
	}

	return 1;
}

BOOL HookFunction(const DWORD originalFn, DWORD hookFn, size_t copyBytes) {
	DWORD OldProtection = { 0 };
	BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)hookFn, copyBytes, PAGE_EXECUTE_READWRITE, &OldProtection);
	if (!success) {
		DWORD error = GetLastError();
		return 0;
	}

	for (size_t i = 0; i < copyBytes; i++) {
		*(BYTE*)((LPBYTE)hookFn + i + 1) = *(BYTE*)((LPBYTE)originalFn + i);
	}

	return PrimitiveHookFunction(originalFn, hookFn, copyBytes);
}

void createHooks() {
	windowWidth = ReadOrDefaultIntFromConfig(SECTION, WIDTH_KEY, DEFAULT_WINDOW_WIDTH);
	windowHeight = ReadOrDefaultIntFromConfig(SECTION, HEIGHT_KEY, DEFAULT_WINDOW_HEIGHT);

    HookFunction(pDirect3DInitFn, (DWORD)Direct3DInitHook, 5);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createHooks, NULL, 0, NULL);
    }
    return TRUE;
}

typedef void* (__stdcall* tDirect3DCreate8)(UINT);
tDirect3DCreate8 original_Direct3DCreate8 = nullptr;

extern "C" __declspec(dllexport) void* __stdcall Direct3DCreate8(UINT SDKVersion) {
	if (!original_Direct3DCreate8) {
		char sysPath[MAX_PATH];
		GetSystemDirectoryA(sysPath, MAX_PATH);
		strcat_s(sysPath, "\\d3d8.dll");

		HMODULE realD3D8 = LoadLibraryA(sysPath);
		original_Direct3DCreate8 = (tDirect3DCreate8)GetProcAddress(realD3D8, "Direct3DCreate8");
	}

	return original_Direct3DCreate8(SDKVersion);
}