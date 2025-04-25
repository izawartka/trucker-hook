#include "common.h"
#include "config.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 720
#define DEFAULT_SCREEN_BITDEPTH 32

static DWORD pDirect3DInitFn = 0x004400a2;
static DWORD pScreenWidth = 0x006d196c;
static DWORD pScreenHeight = 0x006d1968;
static DWORD pScreenBitdepth = 0x006d1970;
static DWORD pScreenUIScale = 0x006d1974;

const char* SECTION = "Screen";
const char* WIDTH_KEY = "Width";
const char* HEIGHT_KEY = "Height";
const char* BITDEPTH_KEY = "BitDepth";

int screenWidth = DEFAULT_SCREEN_WIDTH;
int screenHeight = DEFAULT_SCREEN_HEIGHT;
int screenBitdepth = DEFAULT_SCREEN_BITDEPTH;
float screenUIScale;

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
	*(int*)pScreenWidth = screenWidth;
	*(int*)pScreenHeight = screenHeight;
	*(int*)pScreenBitdepth = screenBitdepth;
	*(float*)pScreenUIScale = screenUIScale;

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

BOOL ReplaceWithNoOp(const DWORD address, size_t bytes) {
	DWORD OldProtection = { 0 };
	BOOL success = VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, bytes, PAGE_EXECUTE_READWRITE, &OldProtection);
	if (!success) {
		DWORD error = GetLastError();
		return 0;
	}

	for (size_t i = 0; i < bytes; i++) {
		*(BYTE*)((LPBYTE)address + i) = 0x90;
	}
}

void createHooks() {
	// read values from config file
	Config::ReadOrDefaultFromConfig(SECTION, WIDTH_KEY, screenWidth, DEFAULT_SCREEN_WIDTH);
	Config::ReadOrDefaultFromConfig(SECTION, HEIGHT_KEY, screenHeight, DEFAULT_SCREEN_HEIGHT);
	Config::ReadOrDefaultFromConfig(SECTION, BITDEPTH_KEY, screenBitdepth, DEFAULT_SCREEN_BITDEPTH);
	screenUIScale = screenHeight / 3.0f * 4.0f * 0.0009765625f;
	
	// disable setting resolution, bitdepth & ui scale by the game
	ReplaceWithNoOp(0x00401edb, 11); // shows current screen width, height, bitdepth in the start menu
	ReplaceWithNoOp(0x004021f3, 6); // sets screen height from the start menu
	ReplaceWithNoOp(0x00402207, 12); // sets screen width & bitdepth from the start menu
	ReplaceWithNoOp(0x00402227, 6); // calculates ui scale from the start menu

	ReplaceWithNoOp(0x0043cc05, 5); // loads screen width from config
	ReplaceWithNoOp(0x0043cc25, 5); // loads screen height from config
	ReplaceWithNoOp(0x0043cc52, 5); // loads screen bitdepth from config
	ReplaceWithNoOp(0x0043da62, 6); // calculates ui scale from config

	*(int*)pScreenWidth = screenWidth;
	*(int*)pScreenHeight = screenHeight;
	*(int*)pScreenBitdepth = screenBitdepth;
	*(float*)pScreenUIScale = screenUIScale;

    //HookFunction(pDirect3DInitFn, (DWORD)Direct3DInitHook, 5);
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